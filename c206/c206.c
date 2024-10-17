/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void)
{
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init(DLList *list)
{
	list->activeElement = NULL;
	list->firstElement = NULL;
	list->lastElement = NULL;
	list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose(DLList *list)
{
	DLLElementPtr currentElem = list->firstElement; // zvolil jsem že bude seznam rušit od začátku do konce
	while (currentElem != NULL)						// zde kontroluje dokola zda nedošel na konec seznamu, nebo jestli není prázdný
	{
		DLLElementPtr nextElem = currentElem->nextElement; // uloží si adresu dalšího prvku seznamu
		free(currentElem);								   // vyčistí současný prvek
		currentElem = nextElem;							   // nastaví jako současný prvek, další z proměnné nextElem
	}
	DLL_Init(list);
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst(DLList *list, long data)
{
	DLLElementPtr newElem = (DLLElementPtr)malloc(sizeof(*newElem)); // vytvoří nový prvek a uloží adresu
	if (newElem == NULL)											 // kontrola zda byl úspěšně vytvořen nový prvek
	{
		DLL_Error();
		exit(1);
	}
	newElem->nextElement = list->firstElement; // zajistí že starý první prvek (pokud existoval) bude uložen v novém jako další prvek
	newElem->previousElement = NULL;
	newElem->data = data;
	if (list->firstElement != NULL)
	{
		list->firstElement->previousElement = newElem;
	}
	// uloží ukazatel nového prvku do bývalého prvního prvku jako předcházející prvek
	// jinak taky nastaví nový prvek jako poseldní v seznamu
	else
	{
		list->lastElement = newElem;
	}
	list->firstElement = newElem; // nastaví nový prvek jako první
	list->currentLength++;		  // incrementace počítadla prvků
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast(DLList *list, long data)
{
	DLLElementPtr newElem = (DLLElementPtr)malloc(sizeof(*newElem)); // vytvoří nový prvek a uloží adresu
	if (newElem == NULL)											 // kontrola zda byl úspěšně vytvořen nový prvek
	{
		DLL_Error();
		exit(1);
	}
	newElem->nextElement = NULL; // zajistí že starý první prvek (pokud existoval) bude uložen v novém jako další prvek
	newElem->previousElement = list->lastElement;
	newElem->data = data;
	if (list->lastElement != NULL)
	{
		list->lastElement->nextElement = newElem;
	}
	// uloží ukazatel nového prvku do bývalého posledního prvku jako následující prvek
	// jinak taky nastaví nový prvek jako poseldní v seznamu
	else
	{
		list->firstElement = newElem;
	}
	list->lastElement = newElem; // nastaví nový prvek jako poslední
	list->currentLength++;		 // incrementace počítadla prvků
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First(DLList *list)
{
	list->activeElement = list->firstElement; // předá ukazatel (na první prvek) ukazatelu (na aktivní prvek)
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last(DLList *list)
{
	list->activeElement = list->lastElement; // předá ukazatel (na poslední prvek) ukazatelu (na aktivní prvek)
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst(DLList *list, long *dataPtr)
{
	if (list->firstElement == NULL) // kontrola jestli existuje první prvek
	{
		DLL_Error();
		return;
	}

	*dataPtr = list->firstElement->data; // uloží data z prvního prvku na místo kam směřuje ukazatel dataPtr
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast(DLList *list, long *dataPtr)
{
	if (list->lastElement == NULL) // kontrola jestli existuje první prvek
	{
		DLL_Error();
		return;
	}

	*dataPtr = list->lastElement->data; // uloží data z posledního prvku na místo kam směřuje ukazatel dataPtr
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst(DLList *list)
{
	DLLElementPtr targetElem = list->firstElement; // vytvoří přímý ukazatel na prvek který bude odstraněn
	if (targetElem != NULL)						   // kontrola zda existuje první prvek
	{
		if (list->activeElement == targetElem)
		{
			list->activeElement = NULL; // pokud je první prvek zároveň aktivní prvek, ztratí se aktivita
		}
		if (list->firstElement == list->lastElement)
		{
			list->firstElement = NULL; // pokud byl v seznamu jediný prvek, nastaví všechny zbylé ukazatele na NULL
			list->lastElement = NULL;
		}
		else
		{
			list->firstElement = targetElem->nextElement; // nastavení ukazatele na nový první prvek
			list->firstElement->previousElement = NULL;	  // NULL jelikož je ve first nový první prvek
		}
		free(targetElem);	   // uvolnění prvku
		list->currentLength--; // snížení počítadla prvků
	}
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast(DLList *list)
{
	DLLElementPtr targetElem = list->lastElement; // vytvoří přímý ukazatel na prvek který bude odstraněn
	if (targetElem != NULL)						  // kontrola zda existuje poslení prvek
	{
		if (list->activeElement == targetElem)
		{
			list->activeElement = NULL; // pokud je první prvek zároveň aktivní prvek, stratí se aktivita
		}
		if (list->firstElement == list->lastElement)
		{
			list->firstElement = NULL; // pokud byl v seznamu jediný prvek, nastaví všechny zbylé ukazatele na NULL
			list->lastElement = NULL;
		}
		else
		{
			list->lastElement = targetElem->previousElement; // nastavení ukazatele na nový poslední prvek
			list->lastElement->nextElement = NULL;			 // NULL jelikož je v last nový poslední prvek
		}
		free(targetElem);	   // uvolnění prvku
		list->currentLength--; // snížení počítadla prvků
	}
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter(DLList *list)
{
	DLLElementPtr activeElem = list->activeElement; // vytvoření přímého ukazatele na aktivní prvek
	if (activeElem != NULL)							// kontrola existence prvku
	{
		DLLElementPtr targetElem = activeElem->nextElement; // vytvoření přímého ukazatele na prvek k odstranění
		if (targetElem != NULL)								// kontrola existence prvku
		{
			activeElem->nextElement = targetElem->nextElement; // nastavení následujícího prvku po aktivním na prvek po odstaňovaném
			if (targetElem == list->lastElement)
			{
				list->lastElement = activeElem; // pokud odstraňujeme poslední prvek, tak nový poslední prvek je aktivní prvek
			}
			else
			{
				activeElem->nextElement->previousElement = activeElem; // update ukazatele nového prvku za aktivním
			}
			free(targetElem);	   // uvolnění prvku
			list->currentLength--; // snížení počítadla prvků
		}
	}
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore(DLList *list)
{
	DLLElementPtr activeElem = list->activeElement; // vytvoření přímého ukazatele na aktivní prvek
	if (activeElem != NULL)							// kontrola existence prvku
	{
		DLLElementPtr targetElem = activeElem->previousElement; // vytvoření přímého ukazatele na prvek k odstranění
		if (targetElem != NULL)									// kontrola existence prvku
		{
			activeElem->previousElement = targetElem->previousElement; // nastavení předcházejícího prvku po aktivním na prvek před odstaňovaném
			if (targetElem == list->firstElement)
			{
				list->firstElement = activeElem; // pokud odstraňujeme první prvek, tak nový první prvek je aktivní prvek
			}
			else
			{
				activeElem->previousElement->nextElement = activeElem; // update ukazatele nového prvku před aktivním
			}
			free(targetElem);	   // uvolnění prvku
			list->currentLength--; // snížení počítadla prvků
		}
	}
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter(DLList *list, long data)
{
	DLLElementPtr activeElem = list->activeElement; // vytvoření přímého ukazatele na aktivní prvek
	if (activeElem != NULL)							// kontrola existence prvku
	{
		DLLElementPtr newElem = (DLLElementPtr)malloc(sizeof(*newElem)); // vytvoří nový prvek a uloží adresu
		if (newElem == NULL)											 // kontrola úspěšného vytvoření
		{
			DLL_Error();
			exit(1);
		}

		if (activeElem == list->lastElement)
		{
			list->lastElement = newElem; // update ukazatele seznamu na poslední prvek
		}
		else
		{
			activeElem->nextElement->previousElement = newElem; // update ukazatele bývalého prvku za aktivním
		}
		newElem->nextElement = activeElem->nextElement; // nastavení nového prvku
		newElem->previousElement = activeElem;
		newElem->data = data;
		activeElem->nextElement = newElem; // nastaví nový prvek jako následníka aktivního
		list->currentLength++;			   // zvýšení počítadla prvků
	}
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore(DLList *list, long data)
{
	DLLElementPtr activeElem = list->activeElement; // vytvoření přímého ukazatele na aktivní prvek
	if (activeElem != NULL)							// kontrola existence prvku
	{
		DLLElementPtr newElem = (DLLElementPtr)malloc(sizeof(*newElem)); // vytvoří nový prvek a uloží adresu
		if (newElem == NULL)											 // kontrola úspěšného vytvoření
		{
			DLL_Error();
			exit(1);
		}

		if (activeElem == list->firstElement)
		{
			list->firstElement = newElem; // update ukazatele seznamu na první prvek
		}
		else
		{
			activeElem->previousElement->nextElement = newElem; // update ukazatele bývalého prvku před aktivním
		}
		newElem->previousElement = activeElem->previousElement; // nastavení nového prvku
		newElem->nextElement = activeElem;
		newElem->data = data;
		activeElem->previousElement = newElem; // nastaví nový prvek jako následníka aktivního
		list->currentLength++;				   // zvýšení počítadla prvků
	}
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue(DLList *list, long *dataPtr)
{
	DLLElementPtr activeElem = list->activeElement; // přímý ukazatel na aktivní prvek
	if (activeElem == NULL)							// kontrola exxistence prvku
	{
		DLL_Error();
		return;
	}

	*dataPtr = activeElem->data; // uložení data aktivního prvku na místo kam ukazuje dataPtr
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue(DLList *list, long data)
{
	DLLElementPtr activeElem = list->activeElement; // přímý ukazatel na aktivní prvek
	if (activeElem != NULL)							// kontrola exxistence prvku
	{
		activeElem->data = data; // nastaví hodnotu data aktivního prvku z parametru data
	}
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next(DLList *list)
{
	DLLElementPtr activeElem = list->activeElement; // přímý ukazatel na aktivní prvek
	if (activeElem != NULL)							// kontrola exxistence prvku
	{
		list->activeElement = activeElem->nextElement; // nastaví ukazatel aktivního prvku na jeho následníka
	}
}

/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous(DLList *list)
{
	DLLElementPtr activeElem = list->activeElement; // přímý ukazatel na aktivní prvek
	if (activeElem != NULL)							// kontrola exxistence prvku
	{
		list->activeElement = activeElem->previousElement; // nastaví ukazatel aktivního prvku na jeho předchůdce
	}
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive(DLList *list)
{
	return (list->activeElement != NULL) ? 1 : 0; // kontrola zda ukazatel aktivního prvku ne/ukazuje na NULL
}

/* Konec c206.c */
