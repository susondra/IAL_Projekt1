/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 *
 * Parametr `packetLists` obsahuje jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Pokud fronta s odpovídající prioritou neexistuje, tato metoda ji alokuje
 * a inicializuje. Za jejich korektní uvolnení odpovídá volající.
 *
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek seznamu.
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 *
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet(DLList *packetLists, PacketPtr packet)
{
    if (packetLists == NULL || packet == NULL) // kontrola zda byly zadány platné ukazatele do parametrů funcke
    {
        return;
    }

    DLL_First(packetLists);           // nastavení aktivity seznamu na začátek
    while (DLL_IsActive(packetLists)) // cyklus dokud nedojde na konec seznamu
    {
        QosPacketListPtr currentPackList = (QosPacketListPtr)packetLists->activeElement->data; // přímý ukazatel na frontu s prioritou
        if (currentPackList->priority == packet->priority)
        // hledá frontu se stejnou prioritou jakou má packet
        {
            DLList *currentList = currentPackList->list;        // přímý ukazatel na seznam paketů ve frontě s prioritou
            if (currentList->currentLength == MAX_PACKET_COUNT) // kontrola zda je seznam plný, případně ho pročistí
            {
                DLL_First(currentList);
                while (DLL_IsActive(currentList))
                {
                    DLL_DeleteAfter(currentList);
                    DLL_Next(currentList);
                }
            }
            DLL_InsertLast(currentList, (long)packet); // vloží packet do příslušného seznamu(fronty)
            return;                                    // zde odchází z funkce
        }
        DLL_Next(packetLists);
    }

    // zde se dostaneme jen pokud neexistuje fronta s příslušnou prioritou k packetu na vstupu
    QosPacketListPtr newPackList = (QosPacketListPtr)malloc(sizeof(QosPacketList)); // vytvoření fronty s prioritou
    if (newPackList == NULL)
    {
        return;
    }
    newPackList->priority = packet->priority;             // přiřazení priority
    newPackList->list = (DLList *)malloc(sizeof(DLList)); // vytvoření seznamu pro frontu
    if (newPackList->list == NULL)
    {
        return;
    }
    DLL_Init(newPackList->list);
    DLL_InsertLast(packetLists, (long)newPackList); // vložení fronty do seznamu (front) 'packetLists'

    receive_packet(packetLists, packet); // rekurzivní volání funkce, aby byl vložen packet do nově vytvořené fronty
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 *
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 *
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Parametr `outputPacketList` obsahuje ukazatele na odeslané pakety (`PacketPtr`).
 *
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets(DLList *packetLists, DLList *outputPacketList, int maxPacketCount)
{
    if (packetLists == NULL || outputPacketList == NULL) // kontrola zda byly zadány platné ukazatele do parametrů funcke
    {
        return;
    }

    if (outputPacketList->currentLength == 0)
    {
        DLL_Init(outputPacketList); // v zadání není napsané, že by se jednalo o inicializovaný seznam, tak ho pro jistotu inicializuju,
                                    // v případě kdyby měl být prázdný, což by mělo zamezit ztrátě dat (samozřejmě by se měl seznam
                                    // inicializovat jen jednou před jeho prvním použitím, ale to se nedá v mém případě nijak pořešit,
                                    // mělo by se to totiž zajistit tam kde se seznam vytváří)
    }

    while (outputPacketList->currentLength < maxPacketCount) // cyklus dokud se nenačte až 'maxPacketCount' packetů,
                                                             // nekonečné zacyklení je pořešeno pomocí break
    {
        char highestPriority = 0;
        DLList *highestPriorityList = NULL; // ukazatel na seznam packetů
        DLL_First(packetLists);             // nastavení aktivity seznamu na začátek
        while (DLL_IsActive(packetLists))   // cyklus dokud nedojde na konec seznamu
        {
            QosPacketListPtr currentPackList = (QosPacketListPtr)packetLists->activeElement->data; // přímý ukazatel na frontu s prioritou
            if (highestPriorityList == NULL || currentPackList->priority > highestPriority)
            // projde pokud je současná fronta s vyšší prioritou než do teď neprázdná nalezená nebo pokud je se jedná o první frontu v seznamu
            {
                DLList *currentList = currentPackList->list; // přímý ukazatel na seznam paketů ve frontě s prioritou
                if (currentList->firstElement != NULL)       // kontrola jestli seznam není prázdný
                {
                    // aktualizuje si seznam packetů a nejvyšší prioritu
                    highestPriorityList = currentList;
                    highestPriority = currentPackList->priority;
                }
            }
            DLL_Next(packetLists); // nastaví aktivitu na další frontu s prioritou v seznamu 'packetLists'
        }
        if (highestPriorityList != NULL) // kontrola zda byl nalezen seznam s packetem
        {
            // získá první packet ze seznamu, vloží ho do výstupního seznamu a odstraní prvek s packetem ze zdroje
            long packet;
            DLL_GetFirst(highestPriorityList, &packet);
            DLL_InsertLast(outputPacketList, packet);
            DLL_DeleteFirst(highestPriorityList);
        }
        else
        {
            break; // přeruší hlední packetů pokud nenašel seznam s packetem
        }
    }
}