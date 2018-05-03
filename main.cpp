//
//  main.cpp
//  trainStationStruct
//
//  Created by Christian Puga on 2/5/18.
//  Copyright © 2018 Christian Puga. All rights reserved.
//

#include <iostream>
#include <stdio.h>

const int MAX_CONNECTION = 100;
const int MAX_STATIONS = 100;
const int MAX_ROUTES = 100;

typedef struct _station {
    char identify;
    struct _connection *listConnections[MAX_CONNECTION];
    int pListConnections;
    
    void constructor(char identify) {
        this->identify = identify;
        this->pListConnections = 0;
    }
    
    bool addConnection(struct _connection *connection) {
        int i;
        bool result = true;
        
        for (i = 0; i < this->pListConnections && result; i++) {
            if (this->listConnections[i] == connection) {
                result = false;
            }
        }
        
        if (result) {
            this->listConnections[this->pListConnections] = connection;
            this->pListConnections++;
        }
        
        return result;
    }
} station;

typedef struct _connection {
    int distanceMetters;
    int speedMaxKiloMettersHours;
    double time;
    _station *station;
    
    void constructor(int distanceMetters, int speedMaxKiloMettersHours, _station *station) {
        float temp;
        
        this->station = station;
        this->distanceMetters = distanceMetters;
        this->speedMaxKiloMettersHours = speedMaxKiloMettersHours;
        
        temp = (double) speedMaxKiloMettersHours * 1000 / 3600;
        this->time = distanceMetters / temp;
    }
} connection;

typedef struct _route {
    _station *listStations[MAX_ROUTES];
    int pListStations = 0;
    bool finish = false;
    bool complete = false;
    
    bool existStation(_station *station) {
        int i;
        bool result = false;
        
        for (i = 0; i < this->pListStations && !result; i++) {
            if (this->listStations[i] == station) {
                result = true;
            }
        }
        
        return result;
    }
    
    void addStation(_station *station) {
        this->listStations[this->pListStations] = station;
        this->pListStations++;
    }
    
    double total(char type) {
        int i, ii;
        double result = 0;
        bool operation;
        
        for (i = 0; i < this->pListStations - 1; i++) {
            operation = false;
            
            for (ii = 0; ii < this->listStations[i]->pListConnections && !operation; ii++) {
                if (this->listStations[i]->listConnections[ii]->station == this->listStations[i + 1]) {
                    operation = true;
                    
                    switch (type) {
                        case 'd':
                            result += this->listStations[i]->listConnections[ii]->distanceMetters;
                            break;
                        
                        case 't':
                            result += this->listStations[i]->listConnections[ii]->time;
                            break;
                            
                        default:
                            operation = false;
                            break;
                    }
                }
            }
        }
        
        return result;
    }
    
    _station *getLastStation() {
        return this->listStations[this->pListStations - 1];
    }
} route;

typedef struct _matrix {
    int y = 0;
    int x = 0;
    double path[MAX_STATIONS][MAX_STATIONS] = {-1};
} matrix;

typedef struct _network {
    _station *listStations[MAX_STATIONS];
    int pListStations;
    
    void constructor() {
        this->pListStations = 0;
    }
    
    _station *existStation(char identity) {
        int i;
        _station *stt = NULL;
        
        for (i = 0; i < this->pListStations && stt == NULL; i++) {
            if (this->listStations[i]->identify == identity) {
                stt = this->listStations[i];
            }
        }
        
        return stt;
    }
    
    bool addStation(_station *station) {
        bool result = false;
        
        if (station != NULL && this->existStation(station->identify) == NULL) {
            this->listStations[this->pListStations] = station;
            this->pListStations++;
            result = true;
        }
        
        return result;
    }
    
    _route calculateRouteMin(_route *route, _station *destitny, char type) {
        int i;
        _route finishRoute, posibleRoute, compareRoute;
        
        if (route->getLastStation()->pListConnections > 0) {
            for (i = 0; i < route->getLastStation()->pListConnections; i++) {
                if (!route->existStation(route->getLastStation()->listConnections[i]->station)) {
                    if (route->getLastStation()->listConnections[i]->station != destitny) {
                        posibleRoute = *route;
                        posibleRoute.addStation(route->getLastStation()->listConnections[i]->station);
                        posibleRoute = this->calculateRouteMin(&posibleRoute, destitny, type);
                        
                        if (posibleRoute.finish && posibleRoute.complete) {
                            compareRoute = posibleRoute;
                        }
                    } else {
                        compareRoute = *route;
                        compareRoute.addStation(route->getLastStation()->listConnections[i]->station);
                        compareRoute.finish = true;
                        compareRoute.complete = true;
                    }
                    
                    if (compareRoute.finish && compareRoute.complete) {
                        if (!finishRoute.complete) {
                            finishRoute = compareRoute;
                        } else if (compareRoute.total(type) < finishRoute.total(type)) {
                            finishRoute = compareRoute;
                        } else if (compareRoute.total(type) == finishRoute.total(type)) {
                            if (compareRoute.pListStations < finishRoute.pListStations) {
                                finishRoute = compareRoute;
                            }
                        }
                    }
                }
            }
        } else {
            finishRoute = *route;
            finishRoute.finish = true;
        }
        
        return finishRoute;
    }
    
    _route createRouteMin(_station *origin, _station *destiny, char type) {
        _route route;
        
        route.addStation(origin);
        route = this->calculateRouteMin(&route, destiny, type);
        
        return route;
    }
    
    _matrix getMatrixDistance(char type) {
        int x, y, i;
        bool put;
        _matrix matrix;
        
        matrix.x = this->pListStations;
        matrix.y = this->pListStations;
        
        for (y = 0; y < matrix.y; y++) {
            for (x = 0; x < matrix.x; x++) {
                if (this->listStations[y] == this->listStations[x]) {
                    matrix.path[y][x] = -1;
                } else {
                    put = false;
                    
                    for (i = 0; i < this->listStations[y]->pListConnections && !put; i++) {
                        if (this->listStations[x] == this->listStations[y]->listConnections[i]->station) {
                            put = true;
                            
                            switch (type) {
                                case 'd':
                                    matrix.path[y][x] = this->listStations[y]->listConnections[i]->distanceMetters;
                                    break;
                                    
                                case 's':
                                    matrix.path[y][x] = this->listStations[y]->listConnections[i]->speedMaxKiloMettersHours;
                                    break;
                                    
                                case 't':
                                    matrix.path[y][x] = this->listStations[y]->listConnections[i]->time;
                                    break;
                                    
                                default:
                                    put = false;
                                    break;
                            }
                        }
                    }
                    
                    if (!put) {
                        matrix.path[y][x] = -1;
                    }
                }
            }
        }
        
        return matrix;
    }
} network;

typedef station *Station;
typedef connection *Connection;
typedef network *Network;
typedef route Route;
typedef matrix Matrix;

/** Create **/
Station createStation(char identify) {
    Station station = NULL;
    
    station = (Station) malloc(sizeof(_station));
    
    if (station != NULL) {
        station->constructor(identify);
    }
    
    return station;
}

Connection createConnection(int distanceMetters, int speedMaxKiloMettersHours, Station station) {
    Connection connection = NULL;
    
    connection = (Connection) malloc(sizeof(_connection));
    
    if (connection != NULL) {
        connection->constructor(distanceMetters, speedMaxKiloMettersHours, station);
    }
    
    return connection;
}

Network createNetwork() {
    Network network = NULL;
    
    network = (Network) malloc(sizeof(_network));
    
    if (network != NULL) {
        network->constructor();
    }
    
    return network;
}

/** Printers **/
void printNetwork(char type, Network net) {
    int i, x, y;
    Matrix matrix;
    
    matrix = net->getMatrixDistance(type);
    
    printf("   |");
    for (i = 0; i < net->pListStations; i++) {
        printf("   %c   |", net->listStations[i]->identify);
    }
    printf("\n");
    printf("---|");
    for (i = 0; i < net->pListStations; i++) {
        printf("-------|");
    }
    printf("\n");
    
    for (y = 0; y < matrix.y; y++) {
        for (x = -1; x < matrix.x; x++) {
            if (x == -1) {
                printf(" %c |", net->listStations[y]->identify);
            } else {
                if (matrix.path[y][x] == (double) -1) {
                    printf("  ---  |");
                } else {
                    printf("%6.2lf |", matrix.path[y][x]);
                }
            }
        }
        
        printf("\n");
    }
}

void printMatrix(Matrix *matrix) {
    int x, y;
    
    for (y = 0; y < matrix->y; y++) {
        for (x = 0; x < matrix->x; x++) {
            printf("%6.2lf", matrix->path[y][x]);
        }
        printf("\n");
    }
}

void printRoute(Route *route) {
    int i;
    
    for (i = 0; i < route->pListStations; i++) {
        printf("%c, ", route->listStations[i]->identify);
    }
}

/** Maintenance **/
void createAndAddStation(char identity, Network network) {
    Station station = NULL;
    
    if (network->existStation(identity) == NULL) {
        station = createStation(identity);
        
        if (station != NULL) {
            network->addStation(station);
        }
    }
}

void createConnectionAndAdd(char identity1, char identity2, int distance, int speedMax, Network network) {
    Station station1 = NULL, station2 = NULL;
    Connection con = NULL;
    
    station1 = network->existStation(identity1);
    station2 = network->existStation(identity2);
    con = createConnection(distance, speedMax, station2);
    station1->addConnection(con);
}

void clearBuffer() {
    std::cin.clear();
    std::cin.ignore(1000, '\n');
}

/** Config **/
void loadDefaultValues(int config, Network net) {
    switch (config) {
        case 0:
            createAndAddStation('A', net);
            createAndAddStation('B', net);
            createAndAddStation('C', net);
            createAndAddStation('D', net);
            createConnectionAndAdd('A', 'B', 20, 50, net);
            createConnectionAndAdd('B', 'C', 20, 50, net);
            createConnectionAndAdd('A', 'D', 30, 50, net);
            createConnectionAndAdd('D', 'C', 40, 50, net);
            createConnectionAndAdd('B', 'D', 10, 50, net);
            break;
            
        case 1:
            createAndAddStation('A', net);
            createAndAddStation('B', net);
            createAndAddStation('C', net);
            createAndAddStation('D', net);
            createAndAddStation('E', net);
            createAndAddStation('F', net);
            createConnectionAndAdd('A', 'D', 9, 50, net);
            createConnectionAndAdd('B', 'A', 3, 50, net);
            createConnectionAndAdd('C', 'B', 1, 50, net);
            createConnectionAndAdd('D', 'B', 6, 50, net);
            createConnectionAndAdd('D', 'C', 4, 50, net);
            createConnectionAndAdd('D', 'E', 8, 50, net);
            createConnectionAndAdd('E', 'F', 7, 50, net);
            createConnectionAndAdd('F', 'C', 9, 50, net);
            createConnectionAndAdd('F', 'D', 11, 50, net);
            break;
    }
}

int main(int argc, const char * argv[]) {
    bool start = true;
    int optionMainMenu, distanceStation, speedStation;
    char identifyStation;
    
    Network net = NULL;
    Station station = NULL, station1 = NULL, station2 = NULL;
    Connection con = NULL;
    Route route;
    Matrix matrix;
    
    net = createNetwork();
    
    loadDefaultValues(0, net);
    
    while (start) {
        printf("Red de estaciones");
        printf("\n1- Crear estacion");
        printf("\n2- Crear camino");
        printf("\n4- Mostrar red");
        printf("\n5- Calculas ruta corta");
        printf("\n9- Cargar network por defecto");
        printf("\n0- Salir");
        printf("\nInseta una opcion: ");
        scanf("%i", &optionMainMenu);
        clearBuffer();
        
        printf("\n");
        
        switch (optionMainMenu) {
            case 0:
                start = false;
                break;
                
            case 1:
                do {
                    printf("Entra el identificador de la estacion: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while (net->existStation(identifyStation) != NULL);
                
                station = NULL;
                station = createStation(identifyStation);
                
                if (station == NULL) {
                    printf("ERROR [C01] No se ha podido crear");
                } else {
                    if (!net->addStation(station)) {
                        printf("ERROR [C02] No se ha podido crear");
                        station = NULL;
                    }
                }
                break;
                
            case 2:
                station1 = NULL;
                station2 = NULL;
                con = NULL;
                
                do {
                    printf("Entra el identificador de la origen: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while ((station1 = net->existStation(identifyStation)) == NULL);
                
                do {
                    printf("Entra el identificador de la destino: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while ((station2 = net->existStation(identifyStation)) == NULL);
                
                do {
                    printf("Entra la distancia: ");
                    scanf("%i", &distanceStation);
                    clearBuffer();
                } while (distanceStation < 1);
                
                do {
                    printf("Entra la velocidad maxima: ");
                    scanf("%i", &speedStation);
                    clearBuffer();
                } while (distanceStation < 1);
                
                con = createConnection(distanceStation, speedStation, station2);
                
                if (con == NULL) {
                    printf("ERROR [L01] No se ha podido crear");
                } else {
                    if (!station1->addConnection(con)) {
                        printf("ERROR [L02] No se ha podido crear");
                        station = NULL;
                    }
                }
                break;
                
            case 4:
                if (net->pListStations > 0) {
                    printNetwork('t', net);
                } else {
                    printf("ERROR [M01] No se ha podido cargar");
                }
                break;
                
            case 5:
                station1 = NULL;
                station2 = NULL;
                con = NULL;
                
                do {
                    printf("Entra el identificador de la origen: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while ((station1 = net->existStation(identifyStation)) == NULL);
                
                do {
                    printf("Entra el identificador de la destino: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while ((station2 = net->existStation(identifyStation)) == NULL);
                
                do {
                    printf("Entra el tipo de ruta a calcular: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while (identifyStation != 't' && identifyStation != 'd');
                
                route = net->createRouteMin(station1, station2, identifyStation);
                
                printf("\nRuta a seguir desde %c a %c: ", station1->identify, station2->identify);
                
                if (route.complete && route.finish) {
                    printRoute(&route);
                    
                    printf("con un");
                    
                    switch (identifyStation) {
                        case 'd':
                            printf("a distancia");
                            break;
                            
                        case 't':
                            printf("tiempo");
                            break;
                    }
                    
                    printf(" total de %.2lf", route.total(identifyStation));
                } else {
                    printf("ERROR [R01] No se ha podido calcular");
                }
                break;
                
            case 9:
                do {
                    printf("Entra una configuracion: ");
                    scanf("%i", &distanceStation);
                    clearBuffer();
                } while (distanceStation < 0);
                
                free(net);
                net = createNetwork();
                loadDefaultValues(distanceStation, net);
                break;
                
            default:
                printf("ERROR No existe esta accion!");
                break;
        }
        
        printf("\n\n");
    }
    
    return 0;
}
