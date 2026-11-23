#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define TEAMS 10
#define MAX_PLAYERS 50

#define BATSMAN 1
#define BOWLER 2
#define ALLROUNDER 3

typedef struct Node {
    int id;
    char name[51];
    int teamId;
    int role;
    int runs;
    float avg;
    float sr;
    int wickets;
    float eco;
    float perf;
    struct Node *next;
} Node;

typedef struct {
    int id;
    char name[51];
    int totalPlayers;
    int battingCount;
    float srSum;
    Node *head[3];
} Team;

Team allTeams[TEAMS];

const char *getRoleName(int role) {
    if (role == BATSMAN) return "Batsman";
    if (role == BOWLER) return "Bowler";
    return "All-rounder";
}

int getRole(const char *text) {
    if (strcmp(text, "Batsman") == 0) return BATSMAN;
    if (strcmp(text, "Bowler") == 0) return BOWLER;
    if (strcmp(text, "All-rounder") == 0) return ALLROUNDER;
    return 0;
}

float getPerformance(int role, float avg, float sr, int wickets, float eco) {
    if (role == BATSMAN) return (avg * sr) / 100;
    if (role == BOWLER) return (wickets * 2) + (100 - eco);
    return ((avg * sr) / 100) + (wickets * 2);
}

Team *findTeam(int id) {
    int low = 0, high = TEAMS - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (allTeams[mid].id == id) return &allTeams[mid];
        if (allTeams[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return NULL;
}

int findTeamIdByName(const char *name) {
    for (int i = 0; i < TEAMS; i++) {
        if (strcmp(allTeams[i].name, name) == 0) return allTeams[i].id;
    }
    return 0;
}

int idExists(int id) {
    for (int i = 0; i < TEAMS; i++) {
        for (int r = 0; r < 3; r++) {
            Node *temp = allTeams[i].head[r];
            while (temp != NULL) {
                if (temp->id == id) return 1;
                temp = temp->next;
            }
        }
    }
    return 0;
}

float teamAvgSr(Team *t) {
    if (t->battingCount == 0) return 0;
    return t->srSum / t->battingCount;
}

void insertPlayer(Team *team, Node *p) {
    int r = p->role - 1;

    if (team->head[r] == NULL || team->head[r]->perf < p->perf) {
        p->next = team->head[r];
        team->head[r] = p;
    } else {
        Node *temp = team->head[r];
        while (temp->next != NULL && temp->next->perf >= p->perf) {
            temp = temp->next;
        }
        p->next = temp->next;
        temp->next = p;
    }

    team->totalPlayers++;
    if (p->role != BOWLER) {
        team->srSum += p->sr;
        team->battingCount++;
    }
}

void loadData() {
    for (int i = 0; i < TEAMS; i++) {
        allTeams[i].id = i + 1;
        strcpy(allTeams[i].name, teams[i]);
        allTeams[i].totalPlayers = 0;
        allTeams[i].battingCount = 0;
        allTeams[i].srSum = 0;
        for (int r = 0; r < 3; r++) allTeams[i].head[r] = NULL;
    }

    for (int i = 0; i < playerCount; i++) {
        int teamId = findTeamIdByName(players[i].team);
        int role = getRole(players[i].role);
        if (teamId == 0 || role == 0) continue;

        Node *p = (Node *)malloc(sizeof(Node));
        if (p == NULL) continue;

        p->id = players[i].id;
        strcpy(p->name, players[i].name);
        p->teamId = teamId;
        p->role = role;
        p->runs = players[i].totalRuns;
        p->avg = players[i].battingAverage;
        p->sr = players[i].strikeRate;
        p->wickets = players[i].wickets;
        p->eco = players[i].economyRate;
        p->perf = getPerformance(role, p->avg, p->sr, p->wickets, p->eco);
        insertPlayer(findTeam(teamId), p);
    }
}

void printHeader(int showTeam) {
    if (showTeam)
        printf("%-5s %-25s %-14s %-12s %6s %6s %6s %5s %5s %10s\n",
               "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    else
        printf("%-5s %-25s %-12s %6s %6s %6s %5s %5s %10s\n",
               "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("--------------------------------------------------------------------------------------------------\n");
}

void printPlayer(Node *p, int showTeam) {
    if (showTeam) {
        Team *t = findTeam(p->teamId);
        printf("%-5d %-25s %-14s %-12s %6d %6.1f %6.1f %5d %5.1f %10.2f\n",
               p->id, p->name, t->name, getRoleName(p->role), p->runs,
               p->avg, p->sr, p->wickets, p->eco, p->perf);
    } else {
        printf("%-5d %-25s %-12s %6d %6.1f %6.1f %5d %5.1f %10.2f\n",
               p->id, p->name, getRoleName(p->role), p->runs,
               p->avg, p->sr, p->wickets, p->eco, p->perf);
    }
}

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int readInt(const char *message, int *value) {
    printf("%s", message);
    if (scanf("%d", value) != 1) {
        clearInput();
        printf("Invalid input.\n");
        return 0;
    }
    return 1;
}

int readFloat(const char *message, float *value) {
    printf("%s", message);
    if (scanf("%f", value) != 1) {
        clearInput();
        printf("Invalid input.\n");
        return 0;
    }
    return 1;
}

void addPlayer() {
    int teamId;
    if (!readInt("Enter Team ID: ", &teamId)) return;

    Team *team = findTeam(teamId);
    if (team == NULL) {
        printf("Team not found.\n");
        return;
    }
    if (team->totalPlayers >= MAX_PLAYERS) {
        printf("Team is full.\n");
        return;
    }

    Node *p = (Node *)malloc(sizeof(Node));
    if (p == NULL) {
        printf("Memory error.\n");
        return;
    }

    printf("Enter Player Details:\n");
    if (!readInt("Player ID: ", &p->id)) { free(p); return; }
    if (p->id < 1 || p->id > 1000 || idExists(p->id)) {
        printf("Player ID is invalid or already used.\n");
        free(p);
        return;
    }

    printf("Name: ");
    scanf(" %50[^\n]", p->name);

    if (!readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ", &p->role) ||
        p->role < 1 || p->role > 3) {
        printf("Invalid role.\n");
        free(p);
        return;
    }
    if (!readInt("Total Runs: ", &p->runs) ||
        !readFloat("Batting Average: ", &p->avg) ||
        !readFloat("Strike Rate: ", &p->sr) ||
        !readInt("Wickets: ", &p->wickets) ||
        !readFloat("Economy Rate: ", &p->eco)) {
        free(p);
        return;
    }

    p->teamId = teamId;
    p->perf = getPerformance(p->role, p->avg, p->sr, p->wickets, p->eco);
    insertPlayer(team, p);
    printf("Player added successfully to Team %s!\n", team->name);
}

void showTeamPlayers() {
    int teamId;
    if (!readInt("Enter Team ID: ", &teamId)) return;

    Team *team = findTeam(teamId);
    if (team == NULL) {
        printf("Team not found.\n");
        return;
    }

    printf("Players of Team %s:\n", team->name);
    printHeader(0);
    for (int r = 0; r < 3; r++) {
        Node *temp = team->head[r];
        while (temp != NULL) {
            printPlayer(temp, 0);
            temp = temp->next;
        }
    }
    printf("Total Players: %d\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamAvgSr(team));
}

void showTeamsBySr() {
    Team *sorted[TEAMS];
    for (int i = 0; i < TEAMS; i++) sorted[i] = &allTeams[i];

    for (int i = 0; i < TEAMS - 1; i++) {
        for (int j = 0; j < TEAMS - i - 1; j++) {
            if (teamAvgSr(sorted[j]) < teamAvgSr(sorted[j + 1])) {
                Team *temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("%-4s %-15s %-10s %-s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("----------------------------------------------\n");
    for (int i = 0; i < TEAMS; i++) {
        printf("%-4d %-15s %-10.2f %d\n", sorted[i]->id, sorted[i]->name,
               teamAvgSr(sorted[i]), sorted[i]->totalPlayers);
    }
}

void showTopK() {
    int teamId, role, k;
    if (!readInt("Enter Team ID: ", &teamId)) return;

    Team *team = findTeam(teamId);
    if (team == NULL) {
        printf("Team not found.\n");
        return;
    }

    if (!readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", &role)) return;
    if (role < 1 || role > 3) {
        printf("Invalid role.\n");
        return;
    }
    if (!readInt("Enter number of players: ", &k)) return;
    if (k <= 0) {
        printf("Number must be greater than 0.\n");
        return;
    }

    printf("Top %d %s players of Team %s:\n", k, getRoleName(role), team->name);
    printHeader(0);
    Node *temp = team->head[role - 1];
    int count = 0;
    while (temp != NULL && count < k) {
        printPlayer(temp, 0);
        temp = temp->next;
        count++;
    }
    if (count == 0) printf("No players found.\n");
}

Node *mergeLists(Node *a, Node *b) {
    Node dummy;
    Node *tail = &dummy;
    dummy.next = NULL;

    while (a != NULL && b != NULL) {
        if (a->perf >= b->perf) {
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    if (a != NULL) tail->next = a;
    else tail->next = b;

    return dummy.next;
}

Node *copyList(Node *head) {
    Node *newHead = NULL, *tail = NULL;
    while (head != NULL) {
        Node *n = (Node *)malloc(sizeof(Node));
        if (n == NULL) break;
        *n = *head;
        n->next = NULL;
        if (newHead == NULL) newHead = n;
        else tail->next = n;
        tail = n;
        head = head->next;
    }
    return newHead;
}

void showAllByRole() {
    int role;
    if (!readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", &role)) return;
    if (role < 1 || role > 3) {
        printf("Invalid role.\n");
        return;
    }

    Node *lists[TEAMS];
    int n = TEAMS;
    for (int i = 0; i < TEAMS; i++) lists[i] = copyList(allTeams[i].head[role - 1]);

    while (n > 1) {
        int newN = 0;
        for (int i = 0; i < n; i += 2) {
            if (i + 1 < n) lists[newN++] = mergeLists(lists[i], lists[i + 1]);
            else lists[newN++] = lists[i];
        }
        n = newN;
    }

    printf("All %s players of all teams:\n", getRoleName(role));
    printHeader(1);
    Node *temp = lists[0];
    if (temp == NULL) printf("No players found.\n");
    while (temp != NULL) {
        printPlayer(temp, 1);
        Node *next = temp->next;
        free(temp);
        temp = next;
    }
}

void freeAll() {
    for (int i = 0; i < TEAMS; i++) {
        for (int r = 0; r < 3; r++) {
            Node *temp = allTeams[i].head[r];
            while (temp != NULL) {
                Node *next = temp->next;
                free(temp);
                temp = next;
            }
        }
    }
}

int main() {
    int choice = 0;
    loadData();

    while (choice != 6) {
        printf("\n==============================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("==============================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printf("==============================================================\n");

        if (!readInt("Enter your choice: ", &choice)) {
            choice = 0;
            continue;
        }

        if (choice == 1) addPlayer();
        else if (choice == 2) showTeamPlayers();
        else if (choice == 3) showTeamsBySr();
        else if (choice == 4) showTopK();
        else if (choice == 5) showAllByRole();
        else if (choice == 6) printf("Exiting...\n");
        else printf("Invalid choice.\n");
    }

    freeAll();
    return 0;
}