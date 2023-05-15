#include <xc.h>
#include "main.h"
#include "matrix_keypad.h"
#include "clcd.h"
#include "eeprom.h"
#include "uart.h"
#include "i2c.h"
#include "matrix_keypad.h"


int log_iter, log_total_saved;

extern char speed[3];
extern unsigned char time[9];


char get_password(void)
{
	CLEAR_DISP_SCREEN;
	clcd_print("ENTER PASSWORD", LINE1(1));
	char entered_password[4] = {0};
	int total_attempts = 5;
	int i = 0;
	int delay = 0;
	unsigned char key_pressed;

	while (total_attempts)
	{
		key_pressed = read_switches(STATE_CHANGE);
		// to avoid bouncing effect
		for (unsigned int wait = 5000; wait--;)
			;

		if (i < 4)
		{
			if (delay++ < 15000)
			{
				clcd_putch('_', LINE2(i));
			}
			else
			{
				clcd_putch(' ', LINE2(i));
			}

			switch (key_pressed)
			{
			case MK_SW11: /*To Enter 1*/
			{
				clcd_print("*", LINE2(i));
				entered_password[i++] = 1;
			}
			break;
			case MK_SW12: /*To Enter 0*/
			{
				clcd_print("*", LINE2(i));
				entered_password[i++] = 0;
			}
			}
		}
		else // if (i == 4)
		{
			int count = 0;

			// compare passwords
			for (int j = 0; j < 4; j++)
			{
				// read password from 100 address
				if (read_external_eeprom(100 + j) == entered_password[j])
					count++;
			}

			if (count == 4)
			{
				CLEAR_DISP_SCREEN;
				return 0;
			}
			else
			{
				i = 0;
				CLEAR_DISP_SCREEN;
				clcd_print("FAILED!", LINE2(4));
				for (int p = 0; p < 10; p++)
					for (unsigned int wait = 50000; wait--;)
						;
				CLEAR_DISP_SCREEN;
				clcd_print("ATTEMPTS LEFT:", LINE1(0));
				clcd_putch('0' + --total_attempts, LINE1(15));
			}
		}
	}
	CLEAR_DISP_SCREEN;
	return 1;
}

void change_password(void)
{
	char exit_flag = 0;
	while (!exit_flag)
	{
		CLEAR_DISP_SCREEN;
		char i = 0;
		char password[4] = {0}, confirm[4] = {0};
		while (i < 4)
		{
			clcd_print("ENTER NEW PWD", LINE1(0));

			switch (read_switches(STATE_CHANGE))
			{
			case MK_SW11: /*Input 1*/
			{
				clcd_putch('*', LINE2(0 + i));
				password[i++] = 1;
			}
			break;
			case MK_SW12: /*Input 0*/
			{
				clcd_putch('*', LINE2(0 + i));
				password[i++] = 0;
			}
			}
		}

		CLEAR_DISP_SCREEN;
		i = 0;

		while (i < 4)
		{
			clcd_print("RE-ENTER PWD!", LINE1(2));

			switch (read_switches(STATE_CHANGE))
			{
			case MK_SW11: /*Input 1*/
			{
				clcd_putch('*', LINE2(0 + i));
				confirm[i++] = 1;
			}
			break;
			case MK_SW12: /*Input 0*/
			{
				clcd_putch('*', LINE2(0 + i));
				confirm[i++] = 0;
			}
			}
		}

		i = 0;
		for (int j = 0; j < 4; j++)
		{
			if (confirm[j] == password[j])
				i++;
		}

		CLEAR_DISP_SCREEN;

		if (i == 4)
		{
			for (int j = 0; j < 4; j++)
				write_external_eeprom(100 + j, password[j]);

			CLEAR_DISP_SCREEN;

			clcd_print("PASSWORD STORED!", LINE1(0));
			clcd_print("SUCCESS!", LINE2(1));

			delay(10);
			CLEAR_DISP_SCREEN;

			exit_flag = 1;
		}
		else
		{
			clcd_print("PWD NOT MATCHING", LINE1(0));
			clcd_print("PLS RE-ENTER!", LINE2(1));
		}

		for (int j = 0; j < 10; j++)
			for (unsigned int wait = 50000; wait--;);
	}
}

void store_key_log(char gear)
{
    write_external_eeprom(log_iter * 10 + 0, time[0]);
    write_external_eeprom(log_iter * 10 + 1, time[1]);
    write_external_eeprom(log_iter * 10 + 2, time[3]);
    write_external_eeprom(log_iter * 10 + 3, time[4]);
    write_external_eeprom(log_iter * 10 + 4, time[6]);
    write_external_eeprom(log_iter * 10 + 5, time[7]);

    if (gear == 'C')
    {
        write_external_eeprom(log_iter * 10 + 6, ' ');
        write_external_eeprom(log_iter * 10 + 7, 'C');
    }
    else if (gear == 'O')
    {
        write_external_eeprom(log_iter * 10 + 6, 'O');
        write_external_eeprom(log_iter * 10 + 7, 'N');
    }
    else if(gear == 'P')
    {
        write_external_eeprom(log_iter * 10 + 6, 'P');
        write_external_eeprom(log_iter * 10 + 7, 'A');
    }
    else if(gear == 'V')
    {
        write_external_eeprom(log_iter * 10 + 6, 'V');
        write_external_eeprom(log_iter * 10 + 7, 'L');
    }
    else if(gear == 'D')
    {
        write_external_eeprom(log_iter * 10 + 6, 'D');
        write_external_eeprom(log_iter * 10 + 7, 'L');
    }
    else if(gear == 'X')
    {
        write_external_eeprom(log_iter * 10 + 6, 'C');
        write_external_eeprom(log_iter * 10 + 7, 'L');
    }
    else if(gear == 'W')
    {
        write_external_eeprom(log_iter * 10 + 6, 'C');
        write_external_eeprom(log_iter * 10 + 7, 'P');
    }
    else
    {
        write_external_eeprom(log_iter * 10 + 6, 'G');
        write_external_eeprom(log_iter * 10 + 7, gear);
    }

    write_external_eeprom(log_iter * 10 + 8, speed[0]);
    write_external_eeprom(log_iter * 10 + 9, speed[1]);

    log_iter = (log_iter + 1) % 10;
    log_total_saved++;
}

void print_to_console(int iter)
{
    putch(read_external_eeprom(iter * 10 + 0));
    putch(read_external_eeprom(iter * 10 + 1));
    putch(':');
    putch(read_external_eeprom(iter * 10 + 2));
    putch(read_external_eeprom(iter * 10 + 3));
    putch(':');
    putch(read_external_eeprom(iter * 10 + 4));
    putch(read_external_eeprom(iter * 10 + 5));
    putch(' ');
    putch(read_external_eeprom(iter * 10 + 6));
    putch(read_external_eeprom(iter * 10 + 7));
    putch(' ');
    putch(read_external_eeprom(iter * 10 + 8));
    putch(read_external_eeprom(iter * 10 + 9));
    putch('\n');
}

void print_log_to_clcd(char LINE, char iter)
{
    clcd_putch(read_external_eeprom(iter * 10 + 0), LINE + 2);
    clcd_putch(read_external_eeprom(iter * 10 + 1), LINE + 3);
    clcd_putch(':', LINE + 4);
    clcd_putch(read_external_eeprom(iter * 10 + 2), LINE + 5);
    clcd_putch(read_external_eeprom(iter * 10 + 3), LINE + 6);
    clcd_putch(':', LINE + 7);
    clcd_putch(read_external_eeprom(iter * 10 + 4), LINE + 8);
    clcd_putch(read_external_eeprom(iter * 10 + 5), LINE + 9);
    clcd_putch(' ', LINE + 10);
    clcd_putch(read_external_eeprom(iter * 10 + 6), LINE + 11);
    clcd_putch(read_external_eeprom(iter * 10 + 7), LINE + 12);
    clcd_putch(' ', LINE + 13);
    clcd_putch(read_external_eeprom(iter * 10 + 8), LINE + 14);
    clcd_putch(read_external_eeprom(iter * 10 + 9), LINE + 15);
}

void view_log(void)
{
    int i = 0, exit_flag = 0;
    char string[16] = {0};

    while (!exit_flag)
    {
        clcd_print("LOGS:", LINE1(0));
        clcd_putch('0' + i, LINE2(0));
        clcd_putch('.', LINE2(1));
        print_log_to_clcd(0xC0, i); // LINE2

        switch (read_switches(STATE_CHANGE))
        {
        case MK_SW11: /*Scroll Up*/
        {
            if (i > 0)
                i--;
        }
        break;
        case MK_SW12: /*Scroll Down*/
        {
            delay(5);
            // long press check
            if (read_switches(LEVEL_CHANGE) == MK_SW12)
                exit_flag = 1;
            else if (log_total_saved > 9)
            {
                if (i < 9)
                    i++;
   
            }
            else if (i < log_iter)
            {
                i++;
            }
        }
        }
    }
}