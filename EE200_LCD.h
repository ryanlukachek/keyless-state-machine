/* 
 * File:   EE200_LCD.h
 * Author: RLukachek
 *
 * Created on November 17, 2020, 5:10 PM
 */

#ifndef EE200_LCD_H
#define	EE200_LCD_H

void Init_LCD_Module(void);
void Position_LCD_Cursor(int cell_num);
void Write_LCD_String(char char_Array[16]);

#endif	/* EE200_LCD_H */

