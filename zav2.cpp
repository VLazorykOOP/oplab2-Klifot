#include <iostream>
#include <vector>
#include <cstdlib>  // для std::rand() і std::srand()
#include <ctime>    // для std::time()
#include <cmath>    // для std::cos() і std::sin()
#include <thread>   // для std::thread
#include <mutex>    // для std::mutex
#include <chrono>   // для std::chrono::milliseconds

const int WIDTH = 800;
const int HEIGHT = 600;
const double M_PI = 3.14159265358979323846;

class Rabbit {
public:
    virtual void move(double dt) = 0;
    virtual void printPosition() const = 0;
};

class RegularRabbit : public Rabbit {
private:
    double x, y;
    double speed;
    double direction;
    double timeToChangeDirection;
    mutable std::mutex mtx;

public:
    RegularRabbit(double speed, double initialX, double initialY)
        : speed(speed), x(initialX), y(initialY), timeToChangeDirection(0) {
        std::srand(static_cast<unsigned int>(std::time(0)));
        changeDirection();
    }

    void changeDirection() {
        direction = (std::rand() % 360) * M_PI / 180.0;  // випадковий напрямок в радіанах
        timeToChangeDirection = std::rand() % 10 + 1;    // випадковий час до наступної зміни напрямку (1-10 секунд)
    }

    void move(double dt) override {
        std::lock_guard<std::mutex> lock(mtx);
        if (timeToChangeDirection <= 0) {
            changeDirection();
        }

        x += speed * std::cos(direction) * dt;
        y += speed * std::sin(direction) * dt;

        if (x < 0) x += WIDTH;
        if (x >= WIDTH) x -= WIDTH;
        if (y < 0) y += HEIGHT;
        if (y >= HEIGHT) y -= HEIGHT;

        timeToChangeDirection -= dt;
    }

    void printPosition() const override {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Regular Rabbit Position: (" << x << ", " << y << ")\n";
    }
};

class AlbinoRabbit : public Rabbit {
private:
    double x;
    double speed;
    mutable std::mutex mtx;

public:
    AlbinoRabbit(double speed, double initialX)
        : speed(speed), x(initialX) {}

    void move(double dt) override {
        std::lock_guard<std::mutex> lock(mtx);
        x += speed * dt;
        if (x < 0) x += WIDTH;
        if (x >= WIDTH) x -= WIDTH;
    }

    void printPosition() const override {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Albino Rabbit Position: (" << x << ", " << HEIGHT / 2 << ")\n";
    }
};

void simulateRabbit(Rabbit& rabbit, double simulationTime, double dt) {
    for (double t = 0; t < simulationTime; t += dt) {
        rabbit.move(dt);
        rabbit.printPosition();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(dt * 1000)));
    }
}

int main() {
    double V = 50;  // швидкість
    double N = 5;   // час до зміни напрямку для звичайних кроликів

    RegularRabbit regularRabbit(V, WIDTH / 2, HEIGHT / 2);
    AlbinoRabbit albinoRabbit(V, WIDTH / 2);

    double simulationTime = 20.0;  // час симуляції в секундах
    double dt = 0.1;               // крок часу в секундах

    // Запуск симуляції в окремих потоках
    std::thread regularRabbitThread(simulateRabbit, std::ref(regularRabbit), simulationTime, dt);
    std::thread albinoRabbitThread(simulateRabbit, std::ref(albinoRabbit), simulationTime, dt);

    // Очікування завершення потоків
    regularRabbitThread.join();
    albinoRabbitThread.join();

    return 0;
}
