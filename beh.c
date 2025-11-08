#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT "BEH.TXT"
#define OUTPUT "vysledkova_listina.txt"
#define SIZE 200

typedef struct {
    int cislo;
    char prijmeni[30];
    char jmeno[30];
    char narozen[15];
    char stat[5];
    char klub[50];
    char cas[10];
    int sekundy;
    int ztrata;
} ZAVODNIK;

int casNaSekundy(char *cas) {
    int h, m, s;
    sscanf(cas, "%d:%d:%d", &h, &m, &s);
    return h * 3600 + m * 60 + s;
}

void sekundyNaCas(int sekundy, char *out) {
    int h = sekundy / 3600;
    int m = (sekundy % 3600) / 60;
    int s = sekundy % 60;
    sprintf(out, "%02d:%02d:%02d", h, m, s);
}

int rokNarozeni(char *datum) {
    int den, mesic, rok;
    sscanf(datum, "%d.%d.%d", &den, &mesic, &rok);
    return rok;
}

ZAVODNIK *nactiSoubor(int *pocet) {
    FILE *fr = fopen(INPUT, "r");
    if (!fr) {
        printf("Chyba: nelze otevrit soubor %s\n", INPUT);
        return NULL;
    }

    ZAVODNIK *zavodnici = NULL;
    char radek[SIZE];
    int i = 0;

    while (fgets(radek, SIZE, fr)) {
        if (strchr(radek, ';') == NULL) continue;

        zavodnici = realloc(zavodnici, (i + 1) * sizeof(ZAVODNIK));
        if (!zavodnici) {
            printf("Chyba alokace pameti\n");
            fclose(fr);
            return NULL;
        }

        sscanf(radek, "%d;%[^;];%[^;];%[^;];%[^;];%[^;];%s",
               &zavodnici[i].cislo,
               zavodnici[i].prijmeni,
               zavodnici[i].jmeno,
               zavodnici[i].narozen,
               zavodnici[i].stat,
               zavodnici[i].klub,
               zavodnici[i].cas);

        zavodnici[i].sekundy = casNaSekundy(zavodnici[i].cas);
        i++;
    }

    fclose(fr);
    *pocet = i;
    return zavodnici;
}

void vypisStartovniListinu(ZAVODNIK *z, int pocet) {
    printf("S T A R T O V N I   L I S T I N A - M A R A T O N\n");
    printf("------------------------------------------------------------\n");
    printf("cislo prijmeni jmeno narozen stat klub cas\n");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < pocet; i++) {
        printf("%4d %-12s %-12s %-12s %-4s %-30s %s\n",
               z[i].cislo, z[i].prijmeni, z[i].jmeno, z[i].narozen, z[i].stat, z[i].klub, z[i].cas);
    }
    printf("------------------------------------------------------------\n");
    printf("Celkovy pocet zavodniku je: %d\n", pocet);
}

void nejstarsiZavodnik(ZAVODNIK *z, int pocet) {
    int minRok = 3000;
    int idx = 0;

    for (int i = 0; i < pocet; i++) {
        int rok = rokNarozeni(z[i].narozen);
        if (rok < minRok) {
            minRok = rok;
            idx = i;
        }
    }

    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    int vek = now->tm_year + 1900 - minRok;

    printf("Nejstarsi zavodnik je %s %s, nar. %s, vek %d let.\n",
           z[idx].jmeno, z[idx].prijmeni, z[idx].narozen, vek);
}

void ceskyZavodnici(ZAVODNIK *z, int pocet) {
    int cz = 0;
    for (int i = 0; i < pocet; i++) {
        if (strcmp(z[i].stat, "CZE") == 0)
            cz++;
    }
    printf("Pocet ceskych zavodniku: %d\n", cz);
}

void seradPodleCasu(ZAVODNIK *z, int pocet) {
    for (int i = 0; i < pocet - 1; i++) {
        for (int j = i + 1; j < pocet; j++) {
            if (z[i].sekundy > z[j].sekundy) {
                ZAVODNIK tmp = z[i];
                z[i] = z[j];
                z[j] = tmp;
            }
        }
    }
}

void vypocetZtrat(ZAVODNIK *z, int pocet) {
    int vedouci = z[0].sekundy;
    for (int i = 0; i < pocet; i++) {
        z[i].ztrata = z[i].sekundy - vedouci;
    }
}

void ulozVysledky(ZAVODNIK *z, int pocet) {
    FILE *fw = fopen(OUTPUT, "w");
    if (!fw) {
        printf("Nelze otevrit soubor %s\n", OUTPUT);
        return;
    }

    fprintf(fw, "V Y S L E D K O V A   L I S T I N A - M A R A T O N\n");
    fprintf(fw, "------------------------------------------------------------\n");
    fprintf(fw, "poradi cislo prijmeni jmeno narozen stat klub cas ztrata\n");
    fprintf(fw, "------------------------------------------------------------\n");

    for (int i = 0; i < pocet; i++) {
        char ztrataCas[10];
        if (z[i].ztrata == 0)
            strcpy(ztrataCas, "00:00:00");
        else
            sekundyNaCas(z[i].ztrata, ztrataCas);

        fprintf(fw, "%2d. %4d %-12s %-12s %-12s %-4s %-25s %8s +%s\n",
                i + 1, z[i].cislo, z[i].prijmeni, z[i].jmeno,
                z[i].narozen, z[i].stat, z[i].klub, z[i].cas, ztrataCas);
    }

    fclose(fw);
    printf("Soubor %s byl ulozen.\n", OUTPUT);
}

int main(void) {
    int pocet;
    ZAVODNIK *zavodnici = nactiSoubor(&pocet);
    if (!zavodnici) return 1;

    vypisStartovniListinu(zavodnici, pocet);
    nejstarsiZavodnik(zavodnici, pocet);
    ceskyZavodnici(zavodnici, pocet);

    seradPodleCasu(zavodnici, pocet);
    vypocetZtrat(zavodnici, pocet);
    ulozVysledky(zavodnici, pocet);

    free(zavodnici);
    return 0;
}
