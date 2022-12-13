#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static pthread_barrier_t barrier;

typedef struct {
    int row;
    int rowSize;
    int **field;
    int result;
} thrData;

void *fieldElements(void *thread_data) {
    thrData *arrayRow = (thrData *) thread_data;
    for (int i = 0; i < arrayRow->rowSize; i++) {
        if (arrayRow->field[arrayRow->row][i]) {
            arrayRow->result += arrayRow->field[arrayRow->row][i];
        }
    }
    return NULL;
}

int main() {
    //Размерности полей боевых действий
    int nA = 0, mA = 0, nT = 0, mT = 0;
    printf("Anchuariya field size:\n");
    if (!scanf("%d %d", &nA, &mA)) {
        printf("Wrong Input\n");
        return -1;
    }
    printf("Taranteria field size:\n");
    if (!scanf("%d %d", &nT, &mT)) {
        printf("Wrong Input\n");
        return -1;
    }

    int projectilePrice = 0;
    printf("Projectile price\n");
    if (!scanf("%d", &projectilePrice)) {
        printf("Wrong Input\n");
        return -1;
    }

    int projectilePower = 0;
    printf("Projectile power\n");
    if (!scanf("%d", &projectilePower)) {
        printf("Wrong Input\n");
        return -1;
    }

    //Создали матрицу поля боевых действий
    printf("Creating war map...\n");
    int **fieldAnchuariya = (int **) malloc(nA * sizeof(int *));
    int **fieldTaranteria = (int **) malloc(nT * sizeof(int *));

    for (int i = 0; i < nA; ++i) {
        fieldAnchuariya[i] = (int *) malloc(mA * sizeof(int));
    }
    for (int i = 0; i < nT; ++i) {
        fieldTaranteria[i] = (int *) malloc(mT * sizeof(int));
    }
    //Заполнение поле боя объектами
    printf("Filling war map...\n");
    time_t t;
    srand((unsigned) time(&t));
    for (int i = 0; i < nA; ++i) {
        for (int j = 0; j < mA; ++j) {
            fieldAnchuariya[i][j] = (rand() % 100 < 30) ? (rand() % 50) : 0;
        }
    }
    for (int i = 0; i < nT; ++i) {
        for (int j = 0; j < mT; ++j) {
            fieldTaranteria[i][j] = (rand() % 100 < 30) ? (rand() % 50) : 0;
        }
    }

    //Создание потоков
    printf("Creating threads...\n");
    pthread_t *threadAnchuariya = (pthread_t *) malloc(nT * sizeof(pthread_t));
    pthread_t *threadTaranteria = (pthread_t *) malloc(nA * sizeof(pthread_t));

    printf("Creating data for threads...\n");
    thrData *SoldiersA = (thrData *) malloc(nT * sizeof(thrData));
    thrData *SoldiersT = (thrData *) malloc(nA * sizeof(thrData));

    int scoreA = 0, scoreT = 0;
    int maxA = 0, maxT = 0;
    int cntA = 0, cntT = 0;

    //Заполнение потоковых данных
    //Anchuariya's soldiers
    printf("Filling threads data...\n");
    for (int i = 0; i < nT; ++i) {
        SoldiersA[i].row = i;
        SoldiersA[i].rowSize = mT;
        SoldiersA[i].field = fieldTaranteria;
        SoldiersA[i].result = 0;
        //Анчуария считает силы Тарантерии
        pthread_create(&threadAnchuariya[i], NULL, fieldElements, &SoldiersA[i]);
    }

    //Taranteria's soldiers
    for (int i = 0; i < nA; ++i) {
        SoldiersT[i].row = i;
        SoldiersT[i].rowSize = mA;
        SoldiersT[i].field = fieldAnchuariya;
        SoldiersT[i].result = 0;
        //Тарантерия считает силы Анчуарии
        pthread_create(&threadTaranteria[i], NULL, fieldElements, &SoldiersT[i]);
        //join
        pthread_join(threadTaranteria[i], NULL);
    }

    printf("Barrier wait...\n");
    pthread_barrier_init(barrier, NULL, nT + nA);
    pthread_barrier_wait(barrier);
    //join
    printf("join threads...\n");
    for (int i = 0; i < nT; ++i) {
        pthread_join(threadAnchuariya[i], NULL);
    }
    for (int i = 0; i < nA; ++i) {
        pthread_join(threadTaranteria[i], NULL);
    }

    //Получаем что есть у противника
    for (int i = 0; i < nT; ++i) {
        maxT += SoldiersA[i].result;
    }
    for (int i = 0; i < nA; ++i) {
        maxA += SoldiersT[i].result;
    }

    printf("Simulating war...\n");
    while (1) {
        //Если все уничтожено то конец
        if (scoreA == maxT) {
            printf("Anchuaria Destroyed Taranteria\n");
            break;
        } else if (scoreT == maxA) {
            printf("Taranteria Destroyed Anchuaria\n");
            break;
        }

        //Ход Анчуарии
        int x = rand() % nT, y = rand() % mT;
        if (fieldTaranteria[x][y] > 0) { //Если попал по цели
            fieldTaranteria[x][y] -= projectilePower;
            scoreA += fieldTaranteria[x][y] > 0 ? projectilePower : projectilePower + fieldTaranteria[x][y];
        }
        cntA += projectilePrice;
        //Проверка
        if (cntA > maxT) {
            printf("Anchuaria spend more than it could destroy\n");
            break;
        }

        //Ход Тарантерии
        x = rand() % nA, y = rand() % mA;
        if (fieldAnchuariya[x][y] > 0) { //Если попал по цели
            fieldAnchuariya[x][y] -= projectilePower;
            scoreT += fieldAnchuariya[x][y] > 0 ? projectilePower : projectilePower + fieldAnchuariya[x][y];
        }
        cntT += projectilePrice;
        //Проверка
        if (cntT > maxA) {
            printf("Taranteria spend more than it could destroy\n");
            break;
        }
    }

    printf("Anchuaria's field units = %d\nTaranteria's field units = %d\nSpend by Anchuaria = %d\nSpend by Taranteria = %d\n",
           maxA, maxT, cntA, cntT);
    printf("Destroyed by Anchuaria = %d\nDestroyed by Taranteria = %d\n", scoreA, scoreT);

    //Освобождение памяти
    pthread_barrier_destroy(barrier);
    free(threadAnchuariya);
    free(threadTaranteria);
    for (int i = 0; i < mA; ++i) {
        free(fieldAnchuariya[i]);
    }
    for (int i = 0; i < mA; ++i) {
        free(fieldTaranteria[i]);
    }
    free(fieldAnchuariya);
    free(fieldTaranteria);
    return 0;
}