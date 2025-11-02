#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT "zavodnici.txt"
#define OUTPUT "vysledkova_listina.txt"
#define SIZE 200

typedef struct {
    int number;
    char surname[30];
    char name[30];
    float firstRun;
    float secondRun;
    float totalTime;
    float timeLoss;
} RACER;

RACER *readFile(int *count) {
    FILE *fr = fopen(INPUT, "r");
    if (!fr) {
        printf("Nelze otevřít vstupní soubor %s\n", INPUT);
        return NULL;
    }

    RACER *racers = NULL;
    char line[SIZE];
    int i = 0;

    while (fgets(line, SIZE, fr)) {
        RACER *temp = realloc(racers, (i + 1) * sizeof(RACER));
        if (!temp) {
            free(racers);
            printf("Chyba alokace paměti\n");
            fclose(fr);
            return NULL;
        }
        racers = temp;

        sscanf(line, "%d %s %s %f %f",
               &racers[i].number,
               racers[i].surname,
               racers[i].name,
               &racers[i].firstRun,
               &racers[i].secondRun);

        racers[i].totalTime = racers[i].firstRun + racers[i].secondRun;
        i++;
    }

    fclose(fr);
    *count = i;
    return racers;
}

void printStartList(RACER *racers, int count) {
    printf("S T A R T O V N I   L I S T I N A - S L A L O M   Ž E N Y\n");
    printf("--------------------------------------------------------\n");
    printf("startovni cislo prijmeni jmeno 1.kolo 2.kolo\n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%6d %12s %12s %.2f %.2f\n",
               racers[i].number,
               racers[i].surname,
               racers[i].name,
               racers[i].firstRun,
               racers[i].secondRun);
    }
}

void sortByTotalTime(RACER *racers, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (racers[i].totalTime > racers[j].totalTime) {
                RACER temp = racers[i];
                racers[i] = racers[j];
                racers[j] = temp;
            }
        }
    }
}

void calculateTimeLoss(RACER *racers, int count) {
    float leaderTime = racers[0].totalTime;
    for (int i = 0; i < count; i++) {
        racers[i].timeLoss = racers[i].totalTime - leaderTime;
    }
}

void saveResults(RACER *racers, int count) {
    FILE *fw = fopen(OUTPUT, "w");
    if (!fw) {
        printf("Nelze otevřít výstupní soubor %s\n", OUTPUT);
        return;
    }

    fprintf(fw, "V Y S L E D K O V A   L I S T I N A\n");
    fprintf(fw, "-------------------------------------------------------------\n");
    fprintf(fw, "poradi(prijmeni jmeno) 1.kolo 2.kolo celkem ztrata\n");
    fprintf(fw, "-------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        fprintf(fw, "%2d %12s %12s %.2f %.2f %.2f %.2f\n",
                i + 1,
                racers[i].surname,
                racers[i].name,
                racers[i].firstRun,
                racers[i].secondRun,
                racers[i].totalTime,
                racers[i].timeLoss);
    }

    fclose(fw);
}

int main(void) {
    int count;
    RACER *racers = readFile(&count);
    if (!racers) return 1;

    printStartList(racers, count);
    sortByTotalTime(racers, count);
    calculateTimeLoss(racers, count);
    saveResults(racers, count);

    free(racers);
    return 0;
}
