#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT "startovni_listina_kombinace_zeny.txt"
#define OUTPUT "vysledkova_listina_kombinace_zeny.txt"
#define SIZE 200
#define DELIM ";,:()\n"

typedef struct {
    int position;
    char name[30];
    char country[5];
    int slalomMin, slalomSec, slalomHund;
    int downhillMin, downhillSec, downhillHund;
    int totalHundredths; // součet v setinách sekundy
} RACER;

int timeToHundredths(int min, int sec, int hund) {
    return min * 6000 + sec * 100 + hund;
}

void hundredthsToTime(int hundredths, int *min, int *sec, int *hund) {
    *min = hundredths / 6000;
    *sec = (hundredths % 6000) / 100;
    *hund = hundredths % 100;
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
        if (i == 0) { i++; continue; } // přeskočí hlavičku

        RACER *temp = realloc(racers, i * sizeof(RACER));
        if (!temp) {
            free(racers);
            printf("Chyba alokace paměti\n");
            fclose(fr);
            return NULL;
        }
        racers = temp;

        char *pch = strtok(line, DELIM);
        int field = 0;
        while (pch) {
            switch (field) {
                case 0: racers[i - 1].position = atoi(pch); break;
                case 1: strcpy(racers[i - 1].name, pch); break;
                case 2: strcpy(racers[i - 1].country, pch); break;
                case 3: racers[i - 1].slalomMin = atoi(pch); break;
                case 4: racers[i - 1].slalomSec = atoi(pch); break;
                case 5: racers[i - 1].slalomHund = atoi(pch); break;
                case 6: racers[i - 1].downhillMin = atoi(pch); break;
                case 7: racers[i - 1].downhillSec = atoi(pch); break;
                case 8: racers[i - 1].downhillHund = atoi(pch); break;
            }
            pch = strtok(NULL, DELIM);
            field++;
        }

        racers[i - 1].totalHundredths =
            timeToHundredths(racers[i - 1].slalomMin, racers[i - 1].slalomSec, racers[i - 1].slalomHund) +
            timeToHundredths(racers[i - 1].downhillMin, racers[i - 1].downhillSec, racers[i - 1].downhillHund);

        i++;
    }

    fclose(fr);
    *count = i - 1;
    return racers;
}

void printStartList(RACER *racers, int count) {
    printf("A L P S K É   L Y Ž O V Á N Í – K O M B I N A C E   Ž E N Y\n");
    printf("----------------------------------------------------------\n");
    printf("poradi jmeno       stat  sjezd slalom  celkem\n");
    printf("----------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        int min, sec, hund;
        hundredthsToTime(racers[i].totalHundredths, &min, &sec, &hund);
        printf("%6d %-12s %4s %02d:%02d,%02d %02d:%02d,%02d %02d:%02d,%02d\n",
               racers[i].position,
               racers[i].name,
               racers[i].country,
               racers[i].downhillMin, racers[i].downhillSec, racers[i].downhillHund,
               racers[i].slalomMin, racers[i].slalomSec, racers[i].slalomHund,
               min, sec, hund);
    }
}

void sortByTotalTime(RACER *racers, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (racers[i].totalHundredths > racers[j].totalHundredths) {
                RACER temp = racers[i];
                racers[i] = racers[j];
                racers[j] = temp;
            }
        }
    }
}

void saveResults(RACER *racers, int count) {
    FILE *fw = fopen(OUTPUT, "w");
    if (!fw) {
        printf("Nelze otevřít výstupní soubor %s\n", OUTPUT);
        return;
    }

    fprintf(fw, "A L P S K É   L Y Ž O V Á N Í – K O M B I N A C E   Ž E N Y\n");
    fprintf(fw, "----------------------------------------------------------\n");
    fprintf(fw, "poradi jmeno       stat  sjezd slalom  celkem ztrata\n");
    fprintf(fw, "----------------------------------------------------------\n");

    int leaderTime = racers[0].totalHundredths;

    for (int i = 0; i < count; i++) {
        int min, sec, hund;
        hundredthsToTime(racers[i].totalHundredths, &min, &sec, &hund);

        int diff = racers[i].totalHundredths - leaderTime;
        int dmin, dsec, dhund;
        hundredthsToTime(diff, &dmin, &dsec, &dhund);

        fprintf(fw, "%6d %-12s %4s %02d:%02d,%02d %02d:%02d,%02d %02d:%02d,%02d +%02d:%02d,%02d\n",
                i + 1,
                racers[i].name,
                racers[i].country,
                racers[i].downhillMin, racers[i].downhillSec, racers[i].downhillHund,
                racers[i].slalomMin, racers[i].slalomSec, racers[i].slalomHund,
                min, sec, hund,
                dmin, dsec, dhund);
    }

    fclose(fw);
}

int main(void) {
    int count;
    RACER *racers = readFile(&count);
    if (!racers) return 1;

    printStartList(racers, count);
    sortByTotalTime(racers, count);
    saveResults(racers, count);

    free(racers);
    return 0;
}
