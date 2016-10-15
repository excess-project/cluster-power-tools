#ifndef FINDBOARD_H_
#define FINDBOARD_H_

/** this function looks up for boards device in the system and opens the devices.
*
* @warning opened device file descriptors are stored in the dynamically created cards array.
* It is the responsibility of the caller to free() it.
*
* @param boards A pointer to the card area to fill
*
* @retval 0 no cards found
* @retval -1 system error (errno gives indication)
* @retval otherwise, number of board found
*/
int apci_find_boards(const char* boardname, int ** boards);

#endif /*FINDBOARD_H_*/
