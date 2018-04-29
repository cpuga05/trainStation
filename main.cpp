//
//  main.cpp
//  trainStation
//
//  Created by Christian Puga on 24/4/18.
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
    int pListConnections = 0;
} station;

typedef struct _connection {
    _station *station;
    int distance;
} connection;

typedef struct _network {
    _station *listStations[MAX_STATIONS];
    int pListStations = 0;
} network;

typedef struct _route {
    _station *listStations[MAX_ROUTES];
    int pListStations = 0;
    bool finish = false;
    bool complete = false;
} route;

typedef station *Station;
typedef connection *Connection;
typedef network *Network;
typedef route Route;

/** Station **/
Station createStation(char identify) {
    Station stt = NULL;
    
    stt = (Station) malloc(sizeof(station));
    
    if (stt != NULL) {
        stt->identify = identify;
        stt->pListConnections = 0;
    }
    
    return stt;
}

/** Connection **/
Connection createConnection(int distance, Station *stt) {
    Connection con = NULL;
    
    con = (Connection) malloc(sizeof(connection));
    
    if (con != NULL) {
        con->distance = distance;
        con->station = (*stt);
    }
    
    return con;
}

bool addConnection(Station *stt, Connection *con) {
    bool result = true;
    int i;
    
    for (i = 0; i < (*stt)->pListConnections && result; i++) {
        if ((*stt)->listConnections[i]->station == (*con)->station) {
            result = false;
        }
    }
    
    if (result) {
        (*stt)->listConnections[(*stt)->pListConnections] = (*con);
        ((*stt)->pListConnections)++;
    }
    
    return result;
}

/** NetWork **/
Network createNetwork() {
    Network net = NULL;
    
    net = (Network) malloc(sizeof(network));
    
    if (net != NULL) {
        net->pListStations = 0;
    }
    
    return net;
}

Station existStation(char identity, Network *net) {
    int i;
    Station stt = NULL;
    
    for (i = 0; i < (*net)->pListStations; i++) {
        if ((*net)->listStations[i]->identify == identity) {
            stt = (*net)->listStations[i];
        }
    }
    
    return stt;
}

bool addStation(Station *station, Network *net) {
    bool result = false;
    
    if (station != NULL && existStation((*station)->identify, &(*net)) == NULL) {
        (*net)->listStations[(*net)->pListStations] = (*station);
        ((*net)->pListStations)++;
        result = true;
    }
    
    return result;
}

void showStation(Network *net) {
    int i;
    
    for (i = 0; i < (*net)->pListStations; i++) {
        printf("Station %02i: %c\n", i, (*net)->listStations[i]->identify);
    }
}

void printNetwork(Network *net) {
    int i, ii, iii;
    bool print = false;
    
    printf("   |");
    for (i = 0; i < (*net)->pListStations; i++) {
        printf(" %c |", (*net)->listStations[i]->identify);
    }
    printf("\n");
    printf("---|");
    for (i = 0; i < (*net)->pListStations; i++) {
        printf("---|");
    }
    printf("\n");
    for (i = 0; i < (*net)->pListStations; i++) {
        printf(" %c |", (*net)->listStations[i]->identify);
        
        for (ii = 0; ii < (*net)->pListStations; ii++) {
            if ((*net)->listStations[i]->identify == (*net)->listStations[ii]->identify) {
                printf(" - |");
            } else {
                print = false;
                
                for (iii = 0; iii < (*net)->listStations[i]->pListConnections; iii++) {
                    if ((*net)->listStations[ii]->identify == (*net)->listStations[i]->listConnections[iii]->station->identify) {
                        print = true;
                        printf("%2i |", (*net)->listStations[i]->listConnections[iii]->distance);
                    }
                }
                
                if (!print) {
                    printf(" - |");
                }
            }
        }
        
        printf("\n");
    }
}

void createMapNetwork(Network *net) {
    int path[MAX_STATIONS][MAX_STATIONS] = {-1};
    int x, y, i;
    bool print;
    
    for (y = 0; y < (*net)->pListStations; y++) {
        for (x = 0; x < (*net)->pListStations; x++) {
            if ((*net)->listStations[y]->identify == (*net)->listStations[x]->identify) {
                path[y][x] = -1;
            } else {
                print = false;
                
                for (i = 0; i < (*net)->listStations[y]->pListConnections; i++) {
                    if ((*net)->listStations[x]->identify == (*net)->listStations[y]->listConnections[i]->station->identify) {
                        print = true;
                        path[y][x] = (*net)->listStations[y]->listConnections[i]->distance;
                    }
                }
                
                if (!print) {
                    path[y][x] = -1;
                }
            }
        }
    }
    
    for (y = 0; y < (*net)->pListStations; y++) {
        for (x = 0; x < (*net)->pListStations; x++) {
            printf("%3i", path[y][x]);
        }
        printf("\n");
    }
}

/** Route **/
bool existInRoute(Station *station, Route *route) {
    int i;
    bool result = false;
    
    for (i = 0; i < route->pListStations && !result; i++) {
        if (route->listStations[i] == (*station)) {
            result = true;
        }
    }
    
    return result;
}

void addRoute(Station *station, Route *route) {
    route->listStations[route->pListStations] = (*station);
    (route->pListStations)++;
}

int routeDistance(Route *route) {
    int i, ii, result = 0;
    
    for (i = 0; i < route->pListStations - 1; i++) {
        for (ii = 0; ii < route->listStations[i]->pListConnections; ii++) {
            if (route->listStations[i]->listConnections[ii]->station == route->listStations[i + 1]) {
                result += route->listStations[i]->listConnections[ii]->distance;
            }
        }
    }
    
    return result;
}

Route calcMin(Route *route, Station *destiny) {
    int i;
    Route finishRoute, posibleRoute, compareRoute;
    
    if (route->listStations[route->pListStations - 1]->pListConnections > 0) {
        for (i = 0; i < route->listStations[route->pListStations - 1]->pListConnections; i++) {
            if (!existInRoute(&(route->listStations[route->pListStations - 1]->listConnections[i]->station), route)) {
                if (route->listStations[route->pListStations - 1]->listConnections[i]->station != (*destiny)) {
                    posibleRoute = (*route);
                    addRoute(&(route->listStations[route->pListStations - 1]->listConnections[i]->station), &posibleRoute);
                    posibleRoute = calcMin(&posibleRoute, &(*destiny));
                    
                    if (posibleRoute.finish && posibleRoute.complete) {
                        compareRoute = posibleRoute;
                    }
                } else {
                    compareRoute = *route;
                    addRoute(&(route->listStations[route->pListStations - 1]->listConnections[i]->station), &compareRoute);
                    compareRoute.finish = true;
                    compareRoute.complete = true;
                }
                
                if (compareRoute.finish && compareRoute.complete) {
                    if (!finishRoute.complete) {
                        finishRoute = compareRoute;
                    } else if (routeDistance(&compareRoute) < routeDistance(&finishRoute)) {
                        finishRoute = compareRoute;
                    } else if (routeDistance(&compareRoute) == routeDistance(&finishRoute)) {
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

Route routeMin(Station *origin, Station *destiny) {
    Route route;
    
    addRoute(&(*origin), &route);
    route = calcMin(&route, &(*destiny));

    return route;
}

void printRoute(Route *route) {
    int i;
    
    for (i = 0; i < route->pListStations; i++) {
        printf("%c, ", route->listStations[i]->identify);
    }
}

/** Maintenance **/
void createAndAddStation(char identity, Network *net) {
    Station station = NULL;
    
    if (existStation(identity, &(*net)) == NULL) {
        station = createStation(identity);
        
        if (station != NULL) {
            addStation(&station, &(*net));
        }
    }
}

void createConnectionAndAdd(char identity1, char identity2, int distance, Network *net) {
    Station station1 = NULL, station2 = NULL;
    Connection con = NULL;
    
    station1 = existStation(identity1, &(*net));
    station2 = existStation(identity2, &(*net));
    con = createConnection(distance, &station2);
    addConnection(&station1, &con);
}

void clearBuffer() {
    std::cin.clear();
    std::cin.ignore(1000, '\n');
}

/** Config **/
void loadDefaultValues(int config, Network *net) {
    switch (config) {
        case 0:
            createAndAddStation('A', &(*net));
            createAndAddStation('B', &(*net));
            createAndAddStation('C', &(*net));
            createAndAddStation('D', &(*net));
            createConnectionAndAdd('A', 'B', 20, &(*net));
            createConnectionAndAdd('B', 'C', 20, &(*net));
            createConnectionAndAdd('A', 'D', 30, &(*net));
            createConnectionAndAdd('D', 'C', 40, &(*net));
            createConnectionAndAdd('B', 'D', 10, &(*net));
            break;
        
        case 1:
            createAndAddStation('A', &(*net));
            createAndAddStation('B', &(*net));
            createAndAddStation('C', &(*net));
            createAndAddStation('D', &(*net));
            createAndAddStation('E', &(*net));
            createAndAddStation('F', &(*net));
            createConnectionAndAdd('A', 'D', 9, &(*net));
            createConnectionAndAdd('B', 'A', 3, &(*net));
            createConnectionAndAdd('C', 'B', 1, &(*net));
            createConnectionAndAdd('D', 'B', 6, &(*net));
            createConnectionAndAdd('D', 'C', 4, &(*net));
            createConnectionAndAdd('D', 'E', 8, &(*net));
            createConnectionAndAdd('E', 'F', 7, &(*net));
            createConnectionAndAdd('F', 'C', 9, &(*net));
            createConnectionAndAdd('F', 'D', 11, &(*net));
            break;
    }
}

int main(int argc, const char * argv[]) {
    bool start = true;
    int optionMainMenu, distanceStation;
    char identifyStation;
    
    Network net = NULL;
    Station station = NULL, station1 = NULL, station2 = NULL;
    Connection con = NULL;
    Route route;
    
    net = createNetwork();
    
    while (start) {
        printf("Red de estaciones");
        printf("\n1- Crear estacion");
        printf("\n2- Crear camino");
        printf("\n3- Listar estaciones");
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
                } while (existStation(identifyStation, &net) != NULL);
                
                station = NULL;
                station = createStation(identifyStation);
                
                if (station == NULL) {
                    printf("ERROR [C01] No se ha podido crear");
                } else {
                    if (!addStation(&station, &net)) {
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
                } while ((station1 = existStation(identifyStation, &net)) == NULL);
                
                do {
                    printf("Entra el identificador de la destino: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while ((station2 = existStation(identifyStation, &net)) == NULL);
                
                do {
                    printf("Entra la distancia: ");
                    scanf("%i", &distanceStation);
                    clearBuffer();
                } while (distanceStation < 1);
                
                con = createConnection(distanceStation, &station2);
                
                if (con == NULL) {
                    printf("ERROR [L01] No se ha podido crear");
                } else {
                    if (!addConnection(&station1, &con)) {
                        printf("ERROR [L02] No se ha podido crear");
                        station = NULL;
                    }
                }
                break;
                
            case 3:
                if (net->pListStations > 0) {
                    showStation(&net);
                } else {
                    printf("ERROR [L01] No se ha podido cargar");
                }
                break;
                
            case 4:
                if (net->pListStations > 0) {
                    printNetwork(&net);
                } else {
                    printf("ERROR [L02] No se ha podido cargar");
                }
//                createMapNetwork(&net);
                break;
                
            case 5:
                station1 = NULL;
                station2 = NULL;
                con = NULL;
                
                do {
                    printf("Entra el identificador de la origen: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while ((station1 = existStation(identifyStation, &net)) == NULL);
                
                do {
                    printf("Entra el identificador de la destino: ");
                    scanf("%c", &identifyStation);
                    clearBuffer();
                } while ((station2 = existStation(identifyStation, &net)) == NULL);
                
                route = routeMin(&station1, &station2);
                
                printf("\nRuta a seguir desde %c a %c: ", station1->identify, station2->identify);
                
                if (route.complete && route.finish) {
                    printRoute(&route);
                    printf("con una distancia total de %i", routeDistance(&route));
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
                loadDefaultValues(distanceStation, &net);
                break;
                
            default:
                printf("ERROR No existe esta accion!");
                break;
        }
        
        printf("\n\n");
    }
    
    return 0;
}
