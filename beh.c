#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT "beh.txt"
#define OUTPUT "vysledkova_listina.txt"
#define SIZE 200
#define DELIM ";:\n"

typedef struct {
    int hour;
    int min;
    int sec;
} TIME;

typedef struct {
    int number;
    char surname[30];
    char name[30];
    int day;
    int month;
    int year;
    char nationality[5];
    char team[50];
    TIME time;
    int totalSeconds;
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
        printf("Nelze otevrit vstupni soubor %s\n", INPUT);
        return NULL;
    }

    RACER *racers = NULL;
    char line[SIZE];
    int i = 0;

    while (fgets(line, SIZE, fr)) {
        RACER *temp = realloc(racers, (i + 1) * sizeof(RACER));
        if (!temp) {
            free(racers);
            printf("Chyba alokace pameti\n");
            fclose(fr);
            return NULL;
        }
        racers = temp;

        char *pch = strtok(line, DELIM);
        int field = 0;
        while (pch) {
            switch (field) {
                case 0: racers[i].number = atoi(pch); break;
                case 1: strcpy(racers[i].surname, pch); break;
                case 2: strcpy(racers[i].name, pch); break;
                case 3: racers[i].day = atoi(pch); break;
                case 4: racers[i].month = atoi(pch); break;
                case 5: racers[i].year = atoi(pch); break;
                case 6: strcpy(racers[i].nationality, pch); break;
                case 7: strcpy(racers[i].team, pch); break;
                case 8: racers[i].time.hour = atoi(pch); break;
                case 9: racers[i].time.min = atoi(pch); break;
                case 10: racers[i].time.sec = atoi(pch); break;
            }
            pch = strtok(NULL, DELIM);
            field++;
        }
        racers[i].totalSeconds = timeToSeconds(racers[i].time);
        i++;
    }

    fclose(fr);
    *count = i;
    return racers;
}

void printStartList(RACER *racers, int count) {
    printf("S T A R T O V N I   L I S T I N A  -  B E H\n");
    printf("---------------------------------------------------------------\n");
    printf("cislo prijmeni jmeno narozeni stat tym cas\n");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%4d %12s %12s %02d.%02d.%04d %4s %-30s %02d:%02d:%02d\n",
               racers[i].number,
               racers[i].surname,
               racers[i].name,
               racers[i].day,
               racers[i].month,
               racers[i].year,
               racers[i].nationality,
               racers[i].team,
               racers[i].time.hour,
               racers[i].time.min,
               racers[i].time.sec);
    }
}

RACER findOldest(RACER *racers, int count) {
    int oldest = 0;
    for (int i = 1; i < count; i++) {
        if (racers[i].year < racers[oldest].year ||
           (racers[i].year == racers[oldest].year && racers[i].month < racers[oldest].month) ||
           (racers[i].year == racers[oldest].year && racers[i].month == racers[oldest].month && racers[i].day < racers[oldest].day)) {
            oldest = i;
        }
    }
    return racers[oldest];
}

int countCzech(RACER *racers, int count) {
    int cze = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(racers[i].nationality, "CZE") == 0) cze++;
    }
    return cze;
}

void sortByTime(RACER *racers, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (racers[i].totalSeconds > racers[j].totalSeconds) {
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
        printf("Nelze otevrit vystupni soubor %s\n", OUTPUT);
        return;
    }

    fprintf(fw, "V Y S L E D K O V A   L I S T I N A\n");
    fprintf(fw, "---------------------------------------------------------------\n");
    fprintf(fw, "poradi prijmeni jmeno stat cas ztrata\n");
    fprintf(fw, "---------------------------------------------------------------\n");

    int leader = racers[0].totalSeconds;
    for (int i = 0; i < count; i++) {
        int diff = racers[i].totalSeconds - leader;
        int h = diff / 3600;
        int m = (diff % 3600) / 60;
        int s = diff % 60;
        fprintf(fw, "%3d %12s %12s %4s %02d:%02d:%02d +%02d:%02d:%02d\n",
                i + 1,
                racers[i].surname,
                racers[i].name,
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
    printf("\nCelkovy pocet zavodniku: %d\n", count);

    RACER oldest = findOldest(racers, count);
    printf("Nejstarsi zavodnik: %s %s, rok narozeni: %d, vek: %d\n",
           oldest.name, oldest.surname, oldest.year, calculateAge(oldest.year));

    printf("Pocet ceskych zavodniku: %d\n", countCzech(racers, count));

    sortByTime(racers, count);
    saveResults(racers, count);

    free(racers);
    return 0;
}
