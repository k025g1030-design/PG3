#include <iostream>
#include <cmath>     
#include <vector>

#define M_PI 3.14159265358979323846

class IShape {
protected:
    double area = 0.0; 

public:
    virtual void Size() = 0;
    virtual void Draw() = 0;
};

class Circle : public IShape {
private:
    double radius; 

public:
    Circle(double r) : radius(r) {}

    void Size() override {
        area = radius * radius * M_PI;
    }

    void Draw() override {
        std::cout << "[円] 半径: " << radius << " -> 面積: " << area << std::endl;
    }
};

class Rectangle : public IShape {
private:
    double width;  
    double height; 

public:
    Rectangle(double w, double h) : width(w), height(h) {}

    void Size() override {
        area = width * height;
    }

    void Draw() override {
        std::cout << "[矩形] 幅: " << width << ", 高さ: " << height << " -> 面積: " << area << std::endl;
    }
};

int main() {
    std::vector<IShape*> shapes;

    shapes.push_back(new Circle(5.0));
    shapes.push_back(new Rectangle(4.0, 5.0));

    for (const auto& shape : shapes) {
        shape->Size();
        shape->Draw(); 
    }

    
    for (auto& shape : shapes) {
        delete shape;
    }

    return 0;
}