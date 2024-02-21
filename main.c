#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATIONS 360

typedef struct {
  char nom[50];
  int ligne;
  int affluence;
  int temps;
} Station;

typedef struct {
  int heure;
  int minutes;
}horraire;


typedef struct {
  int connections[MAX_STATIONS][MAX_STATIONS];
  int num_stations;
  Station stations[MAX_STATIONS];
} reseau;


void initialisation(reseau *metro) {
  int nombre_sommets = 0;
  FILE *plan;
  printf("ouverture fichier texte\n");
  plan = fopen("paris2.txt", "r");
  if (plan == NULL) {
    printf("Impossible d'ouvrir le fichier.\n");
    return;
  }

  fscanf(plan, "%d", &nombre_sommets);
  if (plan == NULL) {
    printf("Error\n");
    return;
  }
  metro->num_stations = nombre_sommets;
  for (int i = 0; i < MAX_STATIONS; ++i) {
    for (int j = 0; j < MAX_STATIONS; ++j) {
      metro->connections[i][j] = 0;
    }
  }
  for (int i = 0; i < metro->num_stations; ++i) {
    int numero_station, numero_ligne, numero_suivant, affluence, temps;
    char nom_station[50];
    fscanf(plan, "%d %s %d %d %d %d", &numero_station, nom_station, &numero_ligne, &numero_suivant, &affluence, &temps);
    strcpy(metro->stations[i].nom, nom_station);
    metro->stations[i].ligne = numero_ligne;
    metro->stations[i].affluence = affluence;
    metro->stations[i].temps = temps;
    if (numero_suivant != -1) { // verifie que c'est pas un terminus
      metro->connections[numero_station][numero_suivant] = 1;
      metro->connections[numero_suivant][numero_station] = 1;
    }
  }

  fclose(plan);
}

void afficherTableau(reseau *metro) {
  printf("Matrice d'adjacence du reseau de metro :\n");
  for (int i = 0; i < metro->num_stations; ++i) {
    for (int j = 0; j < metro->num_stations; ++j) {
      //printf("%d ", metro->connections[i][j]);
    }
    //printf("\n");
  }
}

int stationMinDistance(int distances[], bool visited[], int V) {
  int min = INT_MAX, min_index;

  for (int v = 0; v < V; v++) {
    if (!visited[v] && distances[v] <= min) {
      min = distances[v];
      min_index = v;
    }
  }

  return min_index;
}

void afficherChemin(int parent[], int j, reseau *metro) {
  if (parent[j] == -1) {
    printf("%d ", j);
    return;
  }
  afficherChemin(parent, parent[j], metro);
  printf("-> %d (%s)", j, metro->stations[j].nom);
}


void dijkstra(reseau *metro, int depart, int destination, horraire heureDepart) {
  int V = metro->num_stations;
  int distances[V];
  bool visite[V];
  int parent[V];

  for (int i = 0; i < V; i++) {
    distances[i] = INT_MAX;
    visite[i] = false;
    parent[i] = -1;
  }

  distances[depart] = 0;

  for (int count = 0; count < V - 1; count++) {
    int u = stationMinDistance(distances, visite, V);
    visite[u] = true;

    for (int v = 0; v < V; v++) {
      if (!visite[v] && metro->connections[u][v] && distances[u] != INT_MAX &&
          distances[u] + metro->stations[u].temps < distances[v]) {
        distances[v] = distances[u] + metro->stations[u].temps;
        parent[v] = u;
      }
    }
  }

  int tempsTotal = distances[destination];
  int sommeAffluences = 0;
  int cheminActuel = destination;

  while (cheminActuel != -1) {
    if (parent[cheminActuel] != -1) {
      sommeAffluences += metro->stations[cheminActuel].affluence;
    }
    cheminActuel = parent[cheminActuel];
  }

  int tempsEnMinutes = heureDepart.heure * 60 + heureDepart.minutes + tempsTotal;
  horraire heureArrivee = { tempsEnMinutes / 60, tempsEnMinutes % 60 };
  printf("Le chemin le plus court entre les stations %d et %d avec affluence totale de %d et temps total de %d minutes: ", depart, destination, sommeAffluences, tempsTotal);
  afficherChemin(parent, destination, metro);
  printf("Heure d'arrivée : %02d:%02d\n", heureArrivee.heure, heureArrivee.minutes);
}

//pour verifier si le metro est ouvert
bool verifierHoraireValide(horraire horaire) {
  if ((horaire.heure > 5 && horaire.heure < 24) || (horaire.heure == 1 && horaire.minutes <= 15)) {
    return true;
  } else if (horaire.heure == 5 && horaire.minutes >= 30) {
    return true;
  }
  return false;
}

int main(void) {
  reseau *m = malloc(sizeof(reseau));
  horraire heure;
  initialisation(m);
  afficherTableau(m);
  int depart, destination;
  printf("d'ou partez vous ?");
  scanf("%d", &depart);
  printf("ou allez vous ?");
  scanf("%d", &destination);
  printf("A quelle heure partez-vous ? (HH:MM)\n");
  scanf("%d:%d", &heure.heure, &heure.minutes);
  if (!verifierHoraireValide(heure)) {
    printf("Aucun trajet disponible, les metros circulent de 5h30 à 1h15.\n");
    return 1;
  }
  dijkstra(m, depart, destination, heure);
  return 0;
}




/*void dijkstraAlternatif(reseau *metro, int depart, int destination) {
  int V = metro->num_stations;
  int distances[V];
  bool visite[V];
  int parent[V];

  for (int i = 0; i < V; i++) {
    distances[i] = INT_MAX;
    visite[i] = false;
    parent[i] = -1;
  }

  distances[depart] = 0;

  for (int count = 0; count < V - 1; count++) {
    int u = stationMinDistance(distances, visite, V);
    visite[u] = true;

    for (int v = 0; v < V; v++) {
      if (!visite[v] && metro->connections[u][v] && distances[u] != INT_MAX &&
          distances[u] + metro->connections[u][v] < distances[v]) {
        distances[v] = distances[u] + metro->connections[u][v];
        parent[v] = u;
      }
    }
  }

  int minAffluence = INT_MAX;
  int minStation = destination;
  int totalAffluence = 0;
  minAffluence = INT_MAX;
  int affluenceActuelle = 0;
  int cheminActuel[MAX_STATIONS];

  // Appel de la fonction pour explorer les chemins possibles
  explorerchemins(metro, depart, destination, &minAffluence, &affluenceActuelle, cheminActuel, visite);

  for (int i = destination; i != -1; i = parent[i]) {
    for (int j = i; j != -1; j = parent[j]) {
      totalAffluence += metro->stations[j].affluence;
    }

    if (totalAffluence < minAffluence) {
      minAffluence = totalAffluence;
      minStation = i;
    }
  }

  printf("Le chemin avec la plus petite affluence totale entre les stations %d et %d est:\n ", depart, destination);
  afficherChemin(cheminActuel, destination, metro);
  printf(" avec une affluence totale de %d.\n", minAffluence);
}*/
