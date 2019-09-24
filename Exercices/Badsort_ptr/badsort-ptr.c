#include <stdio.h>

typedef struct {
    char data[4096];
    int key;
} item;

item array[] = {
    {"bill", 3},
    {"neil", 4},
    {"john", 2},
    {"rick", 5},
    {"alex", 1},
};

void sort(item *a, int n) {
    int i = 0, j = 0, c = 0;
    int s = 1;
    item* p;

    for(; (i < n) && (s != 0); i++) {
        s = 0;
        p = a;
        j = n-1;
        c = 1;
        //Añadimos el contador de vueltas c con el que vamos a ir aumentando el puntero. Tambien ajustamos la j para que no se salga de rango
        do {
            if( p->key > (p+1)->key) {
                item t = *p;
                *p  = *(p+1);
                *(p+1) = t;
                s++;
            }
            p = a + c;
            c++;
        } while ( --j > 0 );
    }
}

int main() {
    int i;
    sort(array,5);
    for(i = 0; i < 5; i++)
        printf("array[%d] = {%s, %d}\n",
                i, array[i].data, array[i].key);
    return 0;
}

