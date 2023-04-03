#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>

using namespace std;

int world_width = 10;
int world_height = 10;
double screen_width = 800.0;
double screen_height = 600.0;
double orientation = 315;
double view = 60.0;
double increment_view = 5.0;
double nrays = 800.0;
int view_distance = 20;
double speed = 0.1;

double Pi = 3.141592654;

int world[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };

double x = 1.5;
double y = 1.5;
void draw_mini_map(sf::RenderWindow& window) {
    for (int i = 0; i < world_height; i++) {
        for (int j = 0; j < world_width; j++) {
            sf::RectangleShape rectangle(sf::Vector2f(20, 20));
            rectangle.setPosition(screen_width+j*20, i*20);
            rectangle.setOutlineThickness(1);
            rectangle.setOutlineColor(sf::Color(127, 127, 127));
            if (world[i][j] == 0) {rectangle.setFillColor(sf::Color(0, 0, 0));}
            else {rectangle.setFillColor(sf::Color(255, 255, 255));}
            window.draw(rectangle);
        }
    }
}

void draw_arrow(sf::RenderWindow& window) {
    sf::ConvexShape convex;
    convex.setPointCount(3);
    convex.setPoint(0, sf::Vector2f(0, 0));
    convex.setPoint(1, sf::Vector2f(6, 2));
    convex.setPoint(2, sf::Vector2f(0, 4));
    convex.setFillColor(sf::Color(255, 0, 0));
    convex.setOrigin(3, 2);
    convex.setPosition(screen_width+x*20, y*20);
    convex.setRotation(orientation);
    window.draw(convex);
}

void ray_cast(int i, double angle, double mapping[][4]) {
    double angle2;
    double xrx = x;
    double xry = y;
    double yrx = x;
    double yry = y;
    double lx = 0.0;
    double ly = 0.0;
    double dx;
    double dy;
    if (0 <= angle && angle < 90 || 180 <= angle && angle < 270) {angle2 = ((int)(angle))%90+(angle-((int)(angle)));}
    else {angle2 = 90-((int)(angle))%90-(angle-((int)(angle)));}
    if (angle != 90 && angle != 270) {
        while (1) {
            if (90 < angle && angle < 270) {
                dx = xrx-((int)(xrx));
                xrx = ((int)(xrx))-0.001;
            }
            else {
                dx = 1-(xrx-((int)(xrx)));
                xrx = ((int)(xrx+1.0));
            }
            dy = tan(angle2/180.0*Pi)*dx;
            if (0 <= angle && angle <= 180) {xry += dy;}
            else {xry -= dy;}
            lx += sqrt(dx*dx+dy*dy);
            if (0 < xrx && xrx < world_width && 0 < xry && xry < world_height && lx <= view_distance) {
                if (world[((int)(xry))][((int)(xrx))] != 0) {break;}
            }
            else {
                lx = -1;
                break;
            }
        }
    }
    else {lx = -1;}

    if (90 <= angle && angle < 180 || 270 <= angle && angle < 360) {angle2 = ((int)(angle))%90+(angle-((int)(angle)));}
    else {angle2 = 90-((int)(angle))%90-(angle-((int)(angle)));}
    if (angle != 0 && angle != 180) {
        while (1) {
            if (0 < angle && angle < 180) {
                dy = 1-(yry-((int)(yry)));
                yry = ((int)(yry+1.0));
            }
            else {
                dy = yry-((int)(yry));
                yry = ((int)(yry))-0.001;
            }
            dx = tan(angle2/180.0*Pi)*dy;
            if (90 <= angle && angle <= 270) {yrx -= dx;}
            else {yrx += dx;}
            ly += sqrt(dx*dx+dy*dy);
            if (0 < yrx && yrx < world_width && 0 < yry && yry < world_height && ly <= view_distance) {
                if (world[((int)(yry))][((int)(yrx))] != 0) {break;}
            }
            else {
                ly = -1;
                break;
            }
        }
    }
    else {ly = -1;}

    if (lx < ly && lx != -1 || ly == -1) {
        mapping[i][0] = xrx;
        mapping[i][1] = xry;
        mapping[i][2] = lx;
        mapping[i][3] = 1;
    }
    else {
        mapping[i][0] = yrx;
        mapping[i][1] = yry;
        mapping[i][2] = ly;
        mapping[i][3] = 2;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(screen_width+world_width*20, screen_height), "Ray Casting Test", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    sf::Texture texture;
    texture.loadFromFile("texture.png");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {window.close();}
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            orientation -= increment_view;
            if (orientation == 360) {orientation = 0;}
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            orientation += increment_view;
            if (orientation == -1) {orientation = 359;}
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            x += cos(orientation/180.0*Pi)*speed;
            y += sin(orientation/180.0*Pi)*speed;
            if (world[((int)(y))][((int)(x))] != 0) {
                x -= cos(orientation/180.0*Pi)*speed;
                y -= sin(orientation/180.0*Pi)*speed;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            x -= cos(orientation/180.0*Pi)*speed;
            y -= sin(orientation/180.0*Pi)*speed;
            if (world[((int)(y))][((int)(x))] != 0) {
                x += cos(orientation/180.0*Pi)*speed;
                y += sin(orientation/180.0*Pi)*speed;
            }
        }

        window.clear(sf::Color::Black);

        draw_mini_map(window);

        double mapping[((int)(nrays))][4];
        double angle;
        for (int i = 0; i < nrays; i++) {
            angle = orientation-view/2.0+(view/nrays)*i;
            while (angle < 0) {angle += 360;}
            while (angle >= 360) {angle -= 360;}
            ray_cast(i, angle, mapping);

            // Rayon dans mini map
            sf::RectangleShape ray(sf::Vector2f(mapping[i][2]*20, 1));
            ray.setRotation(orientation-view/2.0+(view/nrays)*i);
            ray.setPosition(screen_width+x*20, y*20);
            ray.setFillColor(sf::Color(255, 255, 0));
            window.draw(ray);

            mapping[i][2] = screen_height/(mapping[i][2]*cos((orientation-angle)/180.0*Pi));
        }

        draw_arrow(window);
        
        sf::RectangleShape sky(sf::Vector2f(screen_width, screen_height/2));
        sky.setPosition(0, 0);
        sky.setFillColor(sf::Color(0, 127, 255));
        window.draw(sky);

        sf::RectangleShape floor(sf::Vector2f(screen_width, screen_height/2));
        floor.setPosition(0, screen_height/2);
        floor.setFillColor(sf::Color(0, 127, 0));
        window.draw(floor);

        int a = 0;
        int b = 0;
        double c;
        for (int i = 0; i < nrays; i++) {
            b++;
            if (!(((int)(mapping[a][0])) == ((int)(mapping[b][0])) && ((int)(mapping[a][1])) == ((int)(mapping[b][1])) && mapping[a][3] == mapping[b][3])) {
                sf::ConvexShape wall;
                wall.setPointCount(4);
                wall.setPoint(0, sf::Vector2f(screen_width/nrays*a, (screen_height-mapping[a][2])/2));
                wall.setPoint(1, sf::Vector2f(screen_width/nrays*b, (screen_height-mapping[b-1][2])/2));
                wall.setPoint(2, sf::Vector2f(screen_width/nrays*b, (screen_height+mapping[b-1][2])/2));
                wall.setPoint(3, sf::Vector2f(screen_width/nrays*a, (screen_height+mapping[a][2])/2));
                if (mapping[a][3] == 1) {wall.setFillColor(sf::Color(255, 0, 0));}
                else {wall.setFillColor(sf::Color(127, 0, 0));}
                window.draw(wall);
                a = b;
            }
        }

        window.display();
    }

    return 0;
}

// sf::VertexArray wall(sf::Quads, 4);
// wall[0].position = sf::Vector2f(screen_width/nrays*a, (screen_height-mapping[a][2])/2);
// wall[1].position = sf::Vector2f(screen_width/nrays*b, (screen_height-mapping[b-1][2])/2);
// wall[2].position = sf::Vector2f(screen_width/nrays*b, (screen_height-mapping[b-1][2])/2+mapping[b-1][2]);
// wall[3].position = sf::Vector2f(screen_width/nrays*a, (screen_height-mapping[a][2])/2+mapping[a][2]);

// Definition zone de la texture à afficher
// wall.setTexture(&texture);
// wall.setFillColor(sf::Color(255, 255, 255));
// window.draw(wall, &texture);