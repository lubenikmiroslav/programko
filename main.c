#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT "CYKLISTIKA.TXT"
#define OUTPUT "vysledkova_listina.txt"
#define SIZE 200
#define DELIM ";,:()\n"

typedef struct {
    int hour;
    int min;
    int sec;
} TIME;

typedef struct {
    int number;
    char surname[30];
    char name[30];
    char nationality[5];
    int year;
    char team[30];
    char category[10];
    TIME time;
} RACER;

int timeToSeconds(TIME t) {
    return t.hour * 3600 + t.min * 60 + t.sec;
}

int calculateAge(int year) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    return local->tm_year + 1900 - year;
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

        RACER *temp = realloc(racers, (i) * sizeof(RACER));
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
                case 0: racers[i - 1].number = atoi(pch); break;
                case 1: strcpy(racers[i - 1].surname, pch); break;
                case 2: strcpy(racers[i - 1].name, pch); break;
                case 3: strcpy(racers[i - 1].nationality, pch); break;
                case 4: racers[i - 1].year = atoi(pch); break;
                case 5: strcpy(racers[i - 1].team, pch); break;
                case 6: strcpy(racers[i - 1].category, pch); break;
                case 7: racers[i - 1].time.hour = atoi(pch); break;
                case 8: racers[i - 1].time.min = atoi(pch); break;
                case 9: racers[i - 1].time.sec = atoi(pch); break;
            }
            pch = strtok(NULL, DELIM);
            field++;
        }
        i++;
    }

    fclose(fr);
    *count = i - 1;
    return racers;
}

void printStartList(RACER *racers, int count) {
    printf("S T A R T O V N I   L I S T I N A - O B R I   B R A S A L\n");
    printf("--------------------------------------------------------\n");
    printf("startovni cislo prijmeni jmeno narodnost rocnik tym kategorie\n");
    printf("--------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%15d %12s %12s %5s %5d %20s %10s\n",
               racers[i].number,
               racers[i].surname,
               racers[i].name,
               racers[i].nationality,
               racers[i].year,
               racers[i].team,
               racers[i].category);
    }
}

RACER findOldest(RACER *racers, int count) {
    int oldest = 0;
    for (int i = 1; i < count; i++) {
        if (racers[i].year < racers[oldest].year)
            oldest = i;
    }
    return racers[oldest];
}

void countByNationality(RACER *racers, int count) {
    int cze = 0, svk = 0, pol = 0, ger = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(racers[i].nationality, "CZE") == 0) cze++;
        else if (strcmp(racers[i].nationality, "SVK") == 0) svk++;
        else if (strcmp(racers[i].nationality, "POL") == 0) pol++;
        else if (strcmp(racers[i].nationality, "GER") == 0) ger++;
    }
    printf("\nPočty závodníků dle národnosti:\n");
    printf("CZE: %d\nSVK: %d\nPOL: %d\nGER: %d\n", cze, svk, pol, ger);
}

void sortByTime(RACER *racers, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (timeToSeconds(racers[i].time) > timeToSeconds(racers[j].time)) {
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

    fprintf(fw, "V Y S L E D K O V A   L I S T I N A\n");
    fprintf(fw, "--------------------------------------------------------\n");
    fprintf(fw, "poradi prijmeni jmeno rocnik stat cas ztrata\n");
    fprintf(fw, "--------------------------------------------------------\n");

    int leaderTime = timeToSeconds(racers[0].time);
    for (int i = 0; i < count; i++) {
        int diff = timeToSeconds(racers[i].time) - leaderTime;
        int h = diff / 3600;
        int m = (diff % 3600) / 60;
        int s = diff % 60;

        fprintf(fw, "%6d %12s %12s %5d %4s %02d:%02d:%02d +%02d:%02d:%02d\n",
                i + 1,
                racers[i].surname,
                racers[i].name,
                racers[i].year,
                racers[i].nationality,
                racers[i].time.hour,
                racers[i].time.min,
                racers[i].time.sec,
                h, m, s);
    }

    fclose(fw);
}

int main(void) {
    int count;
    RACER *racers = readFile(&count);
    if (!racers) return 1;

    printStartList(racers, count);
    printf("\nCelkový počet závodníků: %d\n", count);

    RACER oldest = findOldest(racers, count);
    printf("Nejstarší závodník: %s %s, věk: %d let\n",
           oldest.name, oldest.surname, calculateAge(oldest.year));

    countByNationality(racers, count);
    sortByTime(racers, count);
    saveResults(racers, count);

    free(racers);
    return 0;
}
