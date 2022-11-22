#include "Arbol.h"

///////Arbol
typedef struct sNodo
{
    void *info;
    unsigned tamBytes;
    struct sNodo *izq;
    struct sNodo *der;
}tNodo;


typedef tNodo *Arbol;


void cargarArbolIn(Arbol *pa, const char *nombreArch)
{
    FILE *pArb = fopen(nombreArch, "rb");

    int ls, li, cantRegistros;

    if(!pa)
        return;

    fseek(pArb, 0, SEEK_END); ///PARARSE AL FINAL DEL ARCHIVO
    cantRegistros = ftell(pArb) / sizeof(Idx); ///OBTENER CANTIDAD DE REGISTROS

    li = 0;
    ls = cantRegistros - 1;

    cargarArbolInOrden(pa, pArb, li, ls);

    fclose(pArb);
}


void cargarArbolInOrden(Arbol *pa, FILE *pArb, int li, int ls)
{
    Idx indice;
    int pm;

    if(ls < li)
        return;

    pm = (ls + li) / 2;

    fseek(pArb, pm * sizeof(Idx), SEEK_SET);
    fread(&indice, sizeof(Idx), 1, pArb);

    insertarEnArbol(pa, &indice, sizeof(Idx), compararIdx);
    cargarArbolInOrden(pa, pArb, pm + 1, ls);
    cargarArbolInOrden(pa, pArb, li, pm - 1);

}


void mostrarArchivoBin(const char *nombre,  void *dato, unsigned tamBytes, void mostrar(const void * d1))
{
    FILE *p = fopen(nombre, "rb");
    if(!p)
        return;

    rewind(p);

    fread(dato, tamBytes, 1, p);

    while(!feof(p))
    {
        mostrar(dato);
        fread(dato, tamBytes, 1, p);
    }

    fclose(p);
}


int buscarEmpleadoPorDni(Arbol *pArb, const void *dato, unsigned tamBytes, void *dato2, int cmp(const void *d1, const void *d2))
{
    FILE *pArch = fopen("Empleados.dat", "rb");
    Arbol *pa;
    Idx empIdx;
    void *d1 = malloc(tamBytes);

    if(!d1)
        return 0;

    memcpy(d1, dato, tamBytes);

    if((pa = buscarNodoAElim(pArb, d1, cmp)) != NULL)
    {
        memcpy(&empIdx, (*pa)->info, minimo(sizeof(Idx), (*pa)->tamBytes));
        fseek(pArch, empIdx.pos * sizeof(Empleado) , SEEK_SET);
        fread(dato2, sizeof(Empleado), 1, pArch);

        fclose(pArch);
        free(d1);
        return 1;
    }
    else
    {
        fclose(pArch);
        free(d1);
        return 0;
    }

}


void cargarArbol(Arbol *pa)
{
    FILE *pArch = fopen("Empleados.dat", "rb");
    Empleado emp;
    Idx empIdx;
    int pos = 0;

    if(!pArch)
        return;

    fread(&emp, sizeof(Empleado), 1, pArch);

    empIdx.dni = emp.dni;
    empIdx.pos = pos;

    while (!feof(pArch))
    {
        insertarEnArbol(pa, &empIdx, sizeof(empIdx), compararIdx);
        fread(&emp, sizeof(Empleado), 1, pArch);

        pos++;
        empIdx.dni = emp.dni;
        empIdx.pos = pos;
    }

    fclose(pArch);
}


void grabar(void *d1, FILE *pArch)
{
    Idx *indice = (Idx*)d1;

    fwrite(indice, sizeof(Idx), 1, pArch);
}

void trozarPedidos(char *linea, Pedidos *ped)
{
    char *aux = strchr(linea, '\n');
    *aux = '\0';

    aux = strrchr(linea, ';');
    sscanf(aux + 1, "%d", &ped->cantPedida);
    *aux = '\0';

    aux = strrchr(linea, ';');
    strcpy(ped->codProd, aux + 1);
    *aux = '\0';

    aux = strrchr(linea, ';');
    sscanf(aux + 1, "%d", &ped->codCliente);
    *aux = '\0';

    sscanf(linea, "%d/%d/%d", &ped->fecha.dia, &ped->fecha.mes, &ped->fecha.anio);
}


int actualizarEnArbol(Arbol *pArbol, void *dato, unsigned tamBytes,
                      int(*cmp)(const void *dato1, const void *dato2),
                      void(*acum)(void* dato, void* dato2, void *p), Cola *pc)
{
    int comp; ///variable de compacion

    while(*pArbol)///Mientras HayArbol
    {

        if((comp = cmp(dato, (*pArbol)->info)) < 0)
            pArbol = &(*pArbol)->izq;
        else if(comp > 0)
            pArbol = &(*pArbol)->der;
        else
        {
            acum(dato, (*pArbol)->info, pc);
            return 1;
        }
    }

    return 0;
}


int eliminarElemArbol(Arbol *pa, void *dato, unsigned tambytes, int comp(const void *d1, const void *d2))
{
    if(!(pa = buscarNodoAElim(pa, dato, comp)))
        return 0;
    memcpy(dato, (*pa)->info, minimo(tambytes, (*pa)->tamBytes));

    return eliminarNodoRaiz(pa);
}


tNodo **buscarNodoAElim(const Arbol *pa, const void *dato, int comp(const void *d1, const void *d2))
{
    int rComp;

    while (*pa && (rComp = comp(dato, (*pa)->info)))
    {
        if(rComp < 0)
            pa = &(*pa)->izq;
        else
            pa = &(*pa)->der;
    }

    if(!*pa)
        return NULL;


    return (tNodo **)pa;
}


int eliminarNodoRaiz(Arbol *pa)
{
    tNodo **remp, *elim;

    if(!*pa)
        return 0;

    free((*pa)->info);

    if(!(*pa)->izq && !(*pa)->der)
    {
        free(*pa);
        *pa = NULL;
        return 1;
    }

    remp = alturaArbol(&(*pa)->izq) > alturaArbol(&(*pa)->der) ? mayorNodoArbol(&(*pa)->izq) : menorNodoArbol(&(*pa)->der);

    elim = *remp;
    (*pa)->info = elim->info;
    (*pa)->tamBytes = elim->tamBytes;

    *remp = elim->izq ? elim->izq : elim->der;

    free(elim);

    return 1;
}


tNodo ** menorNodoArbol(const Arbol *pa)
{
    if(!*pa)
        return NULL;
    while ((*pa)->izq)
        pa = &(*pa)->izq;

    return (tNodo **)pa;
}


tNodo ** mayorNodoArbol(const Arbol *pa)
{
    if(!*pa)
        return NULL;
    while ((*pa)->der)
        pa = &(*pa)->der;

    return (tNodo **)pa;
}



int esArbolAvl(const Arbol *pa)
{
    int hd = alturaArbol(&(*pa)->der);
    int hi = alturaArbol(&(*pa)->izq);

    if(!*pa)
        return 1;


    if ((hi - hd) > 1)
        return 0;

    return esArbolAvl(&(*pa)->der) && esArbolAvl(&(*pa)->izq);
}


unsigned alturaArbol(const Arbol *pa)
{
    int hi, hd;

    if(!*pa)
        return 0;
    hi = alturaArbol(&(*pa)->izq);
    hd = alturaArbol(&(*pa)->der);

    return (hi > hd? hi : hd) + 1;
}


void vaciarArbol(Arbol *pa)
{
    if(!*pa)
        return;
    vaciarArbol(&(*pa)->izq);
    vaciarArbol(&(*pa)->der);

    free((*pa)->info);
    free((*pa));

    *pa = NULL;
}


int contarNodos(Arbol *pa)
{
    if(!*pa)
        return 0;
    return contarNodos(&(*pa)->izq) + contarNodos(&(*pa)->der) + 1;
}


void mostrarArbolPosOrden(Arbol *pa, void accion(const void *d1))
{
    if(!*pa)
        return;

    mostrarArbolPosOrden(&(*pa)->izq, accion);
    mostrarArbolPosOrden(&(*pa)->der, accion);
    accion((*pa)->info);
}


void mostrarArbolPreOrden(Arbol *pa, void accion(void *d1))
{
    if(!*pa)
        return;
    accion((*pa)->info);
    mostrarArbolPreOrden(&(*pa)->izq, accion);
    mostrarArbolPreOrden(&(*pa)->der, accion);
}


void guardarIndiceArbol(Arbol *pa, FILE *pARch, void accion(void *d1, FILE *pArch))
{
    if(!*pa)
        return;

    accion((*pa)->info, pARch);
    guardarIndiceArbol(&(*pa)->izq, pARch, accion);
    guardarIndiceArbol(&(*pa)->der, pARch, accion);
}


void guardarIndiceArbolPreOrden(Arbol *pa, void accion(void *d1, FILE *pArch))
{
    FILE *pArch = fopen(ARCH_BIN_IDX, "wb");
    if(!pArch)
        return;
    if(!*pa)
        return;
    guardarIndiceArbol(pa, pArch, accion);
    fclose(pArch);
}


void guardarIndiceArbolIn(Arbol *pa, FILE *pARch, void accion(void *d1, FILE *pArch))
{
    if(!*pa)
        return;

    guardarIndiceArbolIn(&(*pa)->izq, pARch, accion);
    accion((*pa)->info, pARch);
    guardarIndiceArbolIn(&(*pa)->der, pARch, accion);
}


void guardarIndiceArbolInOrden(Arbol *pa, void accion(void *d1, FILE *pArch))
{
    FILE *pArch = fopen("tempIdx.dat", "wb");
    if(!pArch)
        return;
    if(!*pa)
        return;
    guardarIndiceArbolIn(pa, pArch, accion);
    fclose(pArch);
}


void mostrarArbolInOrden(Arbol *pa, int n, void accion(const void *d1))
{
    int i;
    if(!*pa)
        return;

    mostrarArbolInOrden(&(*pa)->izq, n + 1, accion);
    for(i = 0; i < n; i++)
        printf("    ");

    accion((*pa)->info);
    mostrarArbolInOrden(&(*pa)->der, n + 1, accion);
}


int insertarEnArbol(Arbol *pa, const void *dato, unsigned tamBytes, int (cmp)(const void *d1, const void *d2))
{
    tNodo *nue;
    int comp;

    if(*pa)
    {
        if((comp = cmp(dato, (*pa)->info)) < 0)
            return insertarEnArbol(&(*pa)->izq, dato, tamBytes, cmp);
        if(comp > 0)
            return insertarEnArbol(&(*pa)->der, dato, tamBytes, cmp);
        if(comp == 0)
            return 0;
    }

    nue = malloc(sizeof(tNodo));

    if(!nue)
        return 0;
    nue->info = malloc(tamBytes);
    if(!nue->info)
    {
        free(nue);
        return 0;
    }

    memcpy(nue->info, dato, tamBytes);
    nue->tamBytes = tamBytes;

    nue->izq = NULL;
    nue->der = NULL;

    *pa = nue;

    return 1;
}


void crearArbol(Arbol *pa)
{
    *pa = NULL;
}
//// fin arbol


////cola
typedef struct sNodo
{
    void *info;
    unsigned tamBytes;
    struct sNodo *sig;
}tNodo;

typedef tNodo *Cola;


int colaLlena(const Cola *pc, unsigned tamBytes)
{
    tNodo *nue = malloc(sizeof(tNodo));
    void *info = malloc(tamBytes);

    free(nue);
    free(info);

    return nue == NULL || info == NULL;
}


int colaVacia(const Cola *pc)
{
    return *pc == NULL;
}


void vaciarCola(Cola *pc)
{
    while(*pc)
    {
        tNodo *elim = (*pc)->sig;

        if(*pc == NULL) /// si la cola esta vacia
            *pc = NULL;
        else
            (*pc)->sig = elim->sig;

        free(elim->info);
        free(elim);
    }
}


int verPrimero(const Cola *pc, void *dato, unsigned tamBytes)
{
    if(*pc == NULL)
        return 0;
    memcpy(dato, (*pc)->sig->info, minimo(tamBytes, (*pc)->sig->tamBytes));

    return 1;
}


int sacarDeCola(Cola *pc, void *dato, unsigned tamBytes)
{
    tNodo *sacar = (*pc)->sig;

    if(sacar == NULL)
        return 0;

    memcpy(dato, sacar->info, minimo(tamBytes, sacar->tamBytes));

    if(sacar == *pc)
        *pc = NULL;
    else
        (*pc)->sig = sacar->sig;

    free(sacar->info);
    free(sacar);

    return 1;
}


int ponerEnCola(Cola *pc, const void *dato, unsigned tamBytes)
{
    tNodo *poner = malloc(sizeof(tNodo));
    if(poner == NULL)
        return 0;

    poner->info = malloc(tamBytes);
    if(poner->info == NULL)
    {
        free(poner);
        return 0;
    }

    memcpy(poner->info, dato, tamBytes);
    poner->tamBytes = tamBytes;

    if(*pc == NULL) /// si la cola esta vacia
    {
        poner->sig = poner; /// el sig apunta a si mismo
    }
    else
    {
        poner->sig = (*pc)->sig; /// poner va a apuntar al primero de la cola
        (*pc)->sig = poner; /// el sig de la cola apunta a poner
    }
    *pc = poner; ///la cola apunta siempre al ultimo

    return 1;
}


void crearCola(Cola *pc)
{
    *pc = NULL;
}
///////fin cola


////pila
typedef struct sNodo
{
    void *info;
    unsigned tamBytes;
    struct sNodo *sig;
}tNodo;

typedef tNodo *Pila;

int pilaLlena(Pila *pp, unsigned tamBytes)
{
    tNodo *nue = malloc(sizeof(tNodo));
    void *info = malloc(tamBytes);

    free(nue);
    free(info);

    return nue == NULL || info == NULL;
}


int pilaVacia(const Pila *pp)
{
    return *pp == NULL;
}


void vaciarPila(Pila *pp)
{
    while(*pp)
    {
        tNodo *elim = (*pp)->sig;

        if(*pp == elim)
            *pp = NULL;
        else
            (*pp)->sig = elim->sig;

        free(elim->info);
        free(elim);
    }
}


int verTope(const Pila *pp, void *dato, unsigned tamBytes)
{
    if(!*pp)
        return 0;

    memcpy(dato, (*pp)->sig->info, minimo(tamBytes, (*pp)->sig->tamBytes));
    return 1;
}


int desapilar(Pila *pp, void *dato, unsigned tamBytes)
{
    tNodo *sacar = (*pp)->sig;

    if(!sacar)
        return 0;

    memcpy(dato, sacar->info, minimo(tamBytes, sacar->tamBytes));

    if(sacar == *pp) ///hay un solo elemento en la pila
        *pp = NULL;
    else
        (*pp)->sig = sacar->sig; ///apunta al sig de sacar

    free(sacar->info);
    free(sacar);

    return 1;
}


int apilar(Pila *pp, const void *dato, unsigned tamBytes)
{
    tNodo *poner = malloc(sizeof(tNodo));
    if(!poner)
        return 0;

    poner->info = malloc(tamBytes);
    if(!poner->info)
    {
        free(poner);
        return 0;
    }

    memcpy(poner->info, dato, tamBytes);
    poner->tamBytes = tamBytes;

    if(*pp == NULL) ///Si es el primero que pongo
    {
       poner->sig = poner; ///el sig apunta a si mismo
       *pp = poner; /// el puntero a la pila apunta a poner
    }
    else
    {
        poner->sig = (*pp)->sig; ///el sig de poner apunta al sig de la pila
        (*pp)->sig = poner; /// el puntero sig de la pila apunta a poner
    }

    return 1;
}


void crearPila(Pila *pp)
{
   *pp = NULL;
}


////ListaDoble

typedef struct sNodo
{
    void *dato;
    unsigned tamBytes;
    struct sNodo *ant;
    struct sNodo *sig;
}tNodo;

typedef tNodo *Lista;


int eliminarClave(Lista *lista, const void *claveAEliminar, unsigned tamBytes, int buscarClave(const void *dato1, const void *dato2))
{
    tNodo *nodoAEliminar;

    if(*lista == NULL)
        return 0;

    while((*lista)->ant)///Mientras hay anterior
        lista = &(*lista)->ant; ///Posiciono al final de la lista

    while(buscarClave(claveAEliminar, (*lista)->dato) != 0 && (*lista)->sig) ///Mientras no se encuentre la clave y hay un sig
        lista = &(*lista)->sig;

    nodoAEliminar = *lista; ///El nodo a eliminar apunta a la lista

    while(buscarClave(claveAEliminar, (*lista)->dato) == 0) ///Mientras la clave sea igual elimino el nodo
    {

        free(nodoAEliminar->dato); ///Libero el dato
        free(nodoAEliminar); ///Libero el nodo

        *lista = (*lista)->sig;
        nodoAEliminar = *lista;
    }

    return 1;
}


int listaLlena(const Lista *lista,  unsigned tamBytes)
{
    tNodo *nue = (tNodo*)malloc(sizeof(tNodo));
    void *dato = malloc(tamBytes);

    free(nue);
    free(dato);

    return nue == NULL || dato == NULL;
}


void mostrarListaIzq(Lista *lista, void mostrar(const void *d1))
{
    if(*lista == NULL) ///Si la lista esta vacia
        return;

    while((*lista)->ant)///Mientras ant no sea null
        lista = &(*lista)->ant; ///La lista apunta al anterior

    mostrar((*lista)->dato);
    while((*lista)->sig)
    {
        lista = &(*lista)->sig;
        mostrar((*lista)->dato);
    }
}


void mostrarListaDer(Lista *lista, void mostrar(const void *d1))
{
    if(*lista == NULL) ///Si la lista esta vacia
        return;

    while((*lista)->sig)///Mientras sig no sea null
        lista = &(*lista)->sig; ///La lista apunta al sig

    mostrar((*lista)->dato);
    while((*lista)->ant) ///mientra haya anterior
    {
        lista = &(*lista)->ant;
        mostrar((*lista)->dato);
    }

}


int insertarOrdenado(Lista *pl, const void *dato, unsigned tamBytes, int (cmp)(const void *d1, const void *d2))
{
    tNodo *act = *pl, *auxAnt, *auxSig, *nue;

    if(act)
    {
        while(act->sig && cmp(dato, act->dato) > 0)
            act = act->sig;
        while(act->ant && cmp(dato, act->dato) < 0)
            act = act->ant;

        if((cmp(dato, act->dato) < 0))
        {
            auxAnt = act->ant;
            auxSig = act;
        }
        else
        {
            auxSig = act->sig;
            auxAnt = act;
        }
    }
    else
    {
        auxAnt = NULL;
        auxSig = NULL;
    }

    nue = malloc(sizeof(tNodo));
    if(nue == NULL)
        return 0;

    nue->dato = malloc(tamBytes);
    if(nue->dato == NULL)
    {
        free(nue);
        return 0;
    }

    memcpy(nue->dato, dato, tamBytes); ///Copio en el nuevo al dato
    nue->tamBytes = tamBytes; ///Copio el tam

    nue->sig = auxSig;    ///le asigno el puntero sig / ant al nodo nuevo
    nue->ant = auxAnt;

    if(auxAnt) ///si auxAnt es null, es el primero
        auxAnt->sig = nue;

    if(auxSig)///si auxSig es null, es el ultimo
        auxSig->ant = nue;

    *pl = nue;

    return 1;
}


void crearLista(Lista *pl)
{
    *pl = NULL;
}



istream& operator >>(istream &is, StockItem &sto)
{
    char aux[500];

    cout << "Ingrese Nombre" << endl;
    is.getline(aux, 500);
    delete [] sto.nombre;
    sto.nombre = new char[strlen(aux) + 1];
    strcpy(sto.nombre, aux);

    cout << "Ingrese Id" << endl;
    is >> sto.id;

    cout << "Ingrese Precio" << endl;
    is >> sto.precio;

    cout << "Ingrese Cantidad de Articulos" << endl;
    is >> sto.nArticulos;

    return is;
}


Fecha& Fecha::operator ++() //preincremento ++fecha
{
	++this->diaRel;
	return *this;
}


Fecha Fecha::operator ++(int) //postincremento fecha++
{
	Fecha f(*this);
	++this->diaRel;
	return f;
}