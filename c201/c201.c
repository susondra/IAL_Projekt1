/* c201.c **********************************************************************
** Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                              Úpravy: Kamil Jeřábek, září 2020
**                                                    Daniel Dolejška, září 2021
**                                                    Daniel Dolejška, září 2022
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu List.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ List:
**
**      List_Dispose ....... zrušení všech prvků seznamu,
**      List_Init .......... inicializace seznamu před prvním použitím,
**      List_InsertFirst ... vložení prvku na začátek seznamu,
**      List_First ......... nastavení aktivity na první prvek,
**      List_GetFirst ...... vrací hodnotu prvního prvku,
**      List_DeleteFirst ... zruší první prvek seznamu,
**      List_DeleteAfter ... ruší prvek za aktivním prvkem,
**      List_InsertAfter ... vloží nový prvek za aktivní prvek seznamu,
**      List_GetValue ...... vrací hodnotu aktivního prvku,
**      List_SetValue ...... přepíše obsah aktivního prvku novou hodnotou,
**      List_nextElement .......... posune aktivitu na další prvek seznamu,
**      List_IsActive ...... zjišťuje aktivitu seznamu.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**/

#include "c201.h"

#include <stdio.h>	// printf
#include <stdlib.h> // malloc, free

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě. Nastaví error_flag na logickou 1.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void List_Error(void)
{
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným
 * seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
 * že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu jednosměrně vázaného seznamu
 */
void List_Init(List *list)
{
	// prostě inicializace, nastavení všech ukazatelů seznamu na NULL, pro zamezení náhodných předešlých dat v našem listu
	list->activeElement = NULL;
	list->firstElement = NULL;
	list->currentLength = 0; // nastavení počítadla na 0, jelikož je seznam prázdný
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam list do stavu, v jakém se nacházel
 * po inicializaci. Veškerá paměť používaná prvky seznamu list bude korektně
 * uvolněna voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 **/
void List_Dispose(List *list)
{
	ListElementPtr currentElem = list->firstElement; // zvolil jsem že bude seznam rušit od začátku do konce
	while (currentElem != NULL)						 // zde kontroluje dokola zda nedošel na konec seznamu, nebo jestli není prázdný
	{
		ListElementPtr nextElem = currentElem->nextElement; // uloží si adresu dalšího prvku seznamu
		free(currentElem);									// vyčistí současný prvek
		currentElem = nextElem;								// nastaví jako současný prvek, další z proměnné nextElem
	}
	List_Init(list);
}

/**
 * Vloží prvek s hodnotou data na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void List_InsertFirst(List *list, int data)
{
	ListElementPtr newElem = (ListElementPtr)malloc(sizeof(*newElem)); // vytvoří nový prvek a uloží adresu
	if (newElem == NULL)											   // kontrola zda byl úspěšně vytvořen nový prvek
	{
		List_Error();
		exit(1);
	}
	newElem->nextElement = list->firstElement; // zajistí že starý první prvek (pokud existoval) bude uložen v novém jako další prvek
	newElem->data = data;
	list->firstElement = newElem;
	list->currentLength++; // incrementace počítadla prvků
}
/**
 * Nastaví aktivitu seznamu list na jeho první prvek.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_First(List *list)
{
	list->activeElement = list->firstElement; // předá ukazatel (na první prvek) ukazatelu (na aktivní prvek)
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void List_GetFirst(List *list, int *dataPtr)
{
	if (list->firstElement == NULL) // kontrola jestli existuje první prvek
	{
		List_Error();
		return;
	}

	*dataPtr = list->firstElement->data; // uloží data z prvního prvku na místo kam směřuje ukazatel dataPtr
}

/**
 * Zruší první prvek seznamu list a uvolní jím používanou paměť.
 * Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_DeleteFirst(List *list)
{
	ListElementPtr targetElem = list->firstElement; // vytvoří přímý ukazatel na prvek který bude odstraněn
	if (targetElem != NULL)							// kontrola zda existuje první prvek
	{
		if (list->activeElement == targetElem)
		{
			list->activeElement = NULL; // pokud je první prvek zároveň aktivní prvek, ztratí se aktivita
		}
		list->firstElement = targetElem->nextElement; // nastavení ukazatele na nový první prvek
		free(targetElem);							  // uvolnění prvku
		list->currentLength--;						  // snížení počítadla prvků
	}
}

/**
 * Zruší prvek seznamu list za aktivním prvkem a uvolní jím používanou paměť.
 * Pokud není seznam list aktivní nebo pokud je aktivní poslední prvek seznamu list,
 * nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_DeleteAfter(List *list)
{
	ListElementPtr activeElem = list->activeElement; // vytvoření přímého ukazatele na aktivní prvek
	if (activeElem != NULL)							 // kontrola existence prvku
	{
		ListElementPtr targetElem = activeElem->nextElement; // vytvoření přímého ukazatele na prvek k odstranění
		if (targetElem != NULL)								 // kontrola existence prvku
		{
			activeElem->nextElement = targetElem->nextElement; // nastavení nastavení následujícího prvku po aktivním na prvek po odstaňovaném
			free(targetElem);								   // uvolnění prvku
			list->currentLength--;							   // snížení počítadla prvků
		}
	}
}

/**
 * Vloží prvek s hodnotou data za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje!
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * zavolá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void List_InsertAfter(List *list, int data)
{
	ListElementPtr activeElem = list->activeElement; // vytvoření přímého ukazatele na aktivní prvek
	if (activeElem != NULL)							 // kontrola existence prvku
	{
		ListElementPtr newElem = (ListElementPtr)malloc(sizeof(*newElem)); // vytvoří nový prvek a uloží adresu
		if (newElem == NULL)											   // kontrola úspěšného vytvoření
		{
			List_Error();
			exit(1);
		}

		newElem->nextElement = activeElem->nextElement; // nastavení nového prvku
		newElem->data = data;
		activeElem->nextElement = newElem; // nastaví nový prvek jako následníka aktivního
		list->currentLength++;			   // zvýšení počítadla prvků
	}
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam není aktivní, zavolá funkci List_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void List_GetValue(List *list, int *dataPtr)
{
	ListElementPtr activeElem = list->activeElement; // přímý ukazatel na aktivní prvek
	if (activeElem == NULL)							 // kontrola exxistence prvku
	{
		List_Error();
		return;
	}

	*dataPtr = activeElem->data; // uložení data aktivního prvku na místo kam ukazuje dataPtr
}

/**
 * Přepíše data aktivního prvku seznamu list hodnotou data.
 * Pokud seznam list není aktivní, nedělá nic!
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void List_SetValue(List *list, int data)
{
	ListElementPtr activeElem = list->activeElement; // přímý ukazatel na aktivní prvek
	if (activeElem != NULL)							 // kontrola exxistence prvku
	{
		activeElem->data = data; // nastaví hodnotu data aktivního prvku z parametru data
	}
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
 * Pokud není předaný seznam list aktivní, nedělá funkce nic.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
void List_Next(List *list)
{
	ListElementPtr activeElem = list->activeElement; // přímý ukazatel na aktivní prvek
	if (activeElem != NULL)							 // kontrola exxistence prvku
	{
		list->activeElement = activeElem->nextElement; // nastaví ukazatel aktivního prvku na jeho následníka
	}
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Tuto funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu jednosměrně vázaného seznamu
 */
int List_IsActive(List *list)
{
	return (list->activeElement != NULL) ? 1 : 0; // kontrola zda ukazatel aktivního prvku ukazuje neukazuje na NULL
}

/* Konec c201.c */
