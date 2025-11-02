#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT "ZAVODNICI.TXT"
#define OUTPUT "vysledkova_listina.txt"
#define SIZE 300

typedef struct {
    int number;
    char surname[30];
    int terrain;
    int firstRun[10];
    int secondRun[10];
    int sumFirst;
    int sumSecond;
    int total;
} RACER;

int sumRun(int *run) {
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += run[i];
    }
    return sum;
}

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

        char *pch = strtok(line, " \n");
        int field = 0;
        while (pch) {
            if (field == 0) racers[i].number = atoi(pch);
            else if (field == 1) strcpy(racers[i].surname, pch);
            else if (field == 2) racers[i].terrain = atoi(pch);
            else if (field >= 3 && field < 13) racers[i].firstRun[field - 3] = atoi(pch);
            else if (field >= 13 && field < 23) racers[i].secondRun[field - 13] = atoi(pch);
            pch = strtok(NULL, " \n");
            field++;
        }

        racers[i].sumFirst = sumRun(racers[i].firstRun);
        racers[i].sumSecond = sumRun(racers[i].secondRun);
        racers[i].total = racers[i].sumFirst + racers[i].sumSecond;

        i++;
    }

    fclose(fr);
    *count = i;
    return racers;
}

void printStartList(RACER *racers, int count) {
    printf("S T A R T O V N I   L I S T I N A - S L A L O M   Ž E N Y\n");
    printf("--------------------------------------------------------\n");
    printf("startovni cislo | prijmeni\n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%15d | %-12s\n", racers[i].number, racers[i].surname);
    }
}

void saveResults(RACER *racers, int count) {
    FILE *fw = fopen(OUTPUT, "w");
    if (!fw) {
        printf("Nelze otevřít výstupní soubor %s\n", OUTPUT);
        return;
    }

    fprintf(fw, "V Y S L E D K O V A   L I S T I N A - S L A L O M   Ž E N Y\n");
    fprintf(fw, "------------------------------------------------------------\n");
    fprintf(fw, "start. | prijmeni     | 1.kolo | 2.kolo | celkem | trat\n");
    fprintf(fw, "------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        fprintf(fw, "%6d | %-12s | %7d | %7d | %7d | %4d\n",
                racers[i].number,
                racers[i].surname,
                racers[i].sumFirst,
                racers[i].sumSecond,
                racers[i].total,
                racers[i].terrain);
    }

    fclose(fw);
}

int main(void) {
    int count;
    RACER *racers = readFile(&count);
    if (!racers) return 1;

    printStartList(racers, count);
    saveResults(racers, count);

    free(racers);
    return 0;
}
