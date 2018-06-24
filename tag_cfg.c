/*! ------------------------------------------------------------------------------------------------------------------
 * @file    tag_cfg.c
 * @brief   Decawave device configuration and control functions
 *
 * @attention
 *
 * Copyright 2017 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>


#define DATABASE_NAME  "safeguard"
#define DATABASE_USERNAME "test"
#define DATABASE_PASSWORD "1234"
MYSQL *mysql1;



#include "dwm_api.h"
#include "hal.h"


//*****************************************
//*****************************************
//********** CONNECT TO DATABASE **********
//*****************************************
//*****************************************
void mysql_connect (void)
{
    //initialize MYSQL object for connections
    mysql1 = mysql_init(NULL);

    if(mysql1 == NULL)
        {
         fprintf(stderr, "ABB : %s\n", mysql_error(mysql1));
         return;
        }

    //Connect to the database
    if(mysql_real_connect(mysql1, "210.99.149.119", DATABASE_USERNAME, DATABASE_PASSWORD, DATABASE_NAME, 0, NULL, 0) == NULL)
        {
    fprintf(stderr, "%s\n", mysql_error(mysql1));
        }
    else
        {
    printf("Database connection successful.\r\n");
        }
}  
//**********************************************
//**********************************************
//********** DISCONNECT FROM DATABASE **********
//**********************************************
//**********************************************
void mysql_disconnect (void)
{
    mysql_close(mysql1);
     printf( "Disconnected from database.\r\n");
}

int main(void)
{
   mysql_connect();



   char query[255];
   int nQuery_stat;
	


   int i;
   int wait_period = 5000;
   dwm_cfg_tag_t cfg_tag;
   dwm_cfg_t cfg_node;
   
   HAL_Print("dwm_init(): dev%d\n", HAL_DevNum());
   dwm_init();
   
   HAL_Print("Setting to tag: dev%d.\n", HAL_DevNum());
   cfg_tag.low_power_en = 0; 
   cfg_tag.meas_mode = DWM_MEAS_MODE_TWR;
   cfg_tag.loc_engine_en = 1;
   cfg_tag.common.led_en = 1;
   cfg_tag.common.ble_en = 1;
   cfg_tag.common.uwb_mode = DWM_UWB_MODE_ACTIVE;
   cfg_tag.common.fw_update_en = 0;
   HAL_Print("dwm_cfg_tag_set(&cfg_tag): dev%d.\n", HAL_DevNum());
   dwm_cfg_tag_set(&cfg_tag);
   
   HAL_Print("Wait 2s for node to reset.\n");
   HAL_Delay(1000);  
   dwm_cfg_get(&cfg_node);
   
   HAL_Print("Comparing set vs. get: dev%d.\n", HAL_DevNum());
   if((cfg_tag.low_power_en        != cfg_node.low_power_en) 
   || (cfg_tag.meas_mode           != cfg_node.meas_mode) 
   || (cfg_tag.loc_engine_en       != cfg_node.loc_engine_en) 
   || (cfg_tag.common.led_en       != cfg_node.common.led_en) 
   || (cfg_tag.common.ble_en       != cfg_node.common.ble_en) 
   || (cfg_tag.common.uwb_mode     != cfg_node.common.uwb_mode) 
   || (cfg_tag.common.fw_update_en != cfg_node.common.fw_update_en))
   {
      HAL_Print("low_power_en        cfg_tag=%d : cfg_node=%d\n", cfg_tag.low_power_en,     cfg_node.low_power_en); 
      HAL_Print("meas_mode           cfg_tag=%d : cfg_node=%d\n", cfg_tag.meas_mode,        cfg_node.meas_mode); 
      HAL_Print("loc_engine_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.loc_engine_en,    cfg_node.loc_engine_en); 
      HAL_Print("common.led_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.led_en,    cfg_node.common.led_en); 
      HAL_Print("common.ble_en       cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.ble_en,    cfg_node.common.ble_en); 
      HAL_Print("common.uwb_mode     cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.uwb_mode,  cfg_node.common.uwb_mode); 
      HAL_Print("common.fw_update_en cfg_tag=%d : cfg_node=%d\n", cfg_tag.common.fw_update_en, cfg_node.common.fw_update_en);  
      HAL_Print("\nConfiguration failed.\n\n");
	  
   }
   else
   {
      HAL_Print("\nConfiguration succeeded.\n\n");
   }
   
   dwm_loc_data_t loc;
   dwm_pos_t pos;
   loc.p_pos = &pos;
   while(1)
   {      
      HAL_Print("Wait %d ms...\n", wait_period);
      HAL_Delay(wait_period);        
   
      HAL_Print("dwm_loc_get(&loc):\n");
      if(dwm_loc_get(&loc) == RV_OK)
      {
         for (i = 0; i < loc.anchors.dist.cnt; ++i) 
         {
            HAL_Print("\t%u)", i);
            HAL_Print("0x%llx", loc.anchors.dist.addr[i]);
            if (i < loc.anchors.an_pos.cnt) 
            {
               HAL_Print("[%d,%d]", loc.anchors.an_pos.pos[i].x,
                     loc.anchors.an_pos.pos[i].y);
            }
            HAL_Print("=%u,%u\n", loc.anchors.dist.dist[i], loc.anchors.dist.qf[i]);

			// query¹® ÀÛ¼º
			sprintf(query,"insert into distap(id, x, y, dist) values('%d', '%d', '%d', '%d')",i,loc.anchors.an_pos.pos[i].x, loc.anchors.an_pos.pos[i].y, loc.anchors.dist.dist[i]);
			nQuery_stat = mysql_query(mysql1, query);
			if(nQuery_stat != 0)
			{
				printf("\n------error------\n");
			}

		 }
      }
   }
   
   return 0;   
}
