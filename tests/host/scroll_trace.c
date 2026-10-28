#include <stdio.h>
#include <string.h>
#include TARGET_TRACKBALL
uint16_t mock_now;
int main(void){
  for(int step=1;step<=8;step*=2){
    memset(distances,0,sizeof distances);memset(rate_meters,0,sizeof rate_meters);
    memset(gliders,0,sizeof gliders);memset(wheel_buffer,0,sizeof wheel_buffer);
    last_mode=MODE_MOUSE;last_report=0;precision_mode=false;select_button_pressed=false;
    unsigned rng=12345;
    for(int t=0;t<12000;t++){
      mock_now=t;select_button_pressed=(t/3000)%2;
      if(t%1000==0)precision_mode=!precision_mode;
      rng=rng*1664525u+1013904223u;
      if((rng>>24)<40)trackball_move((rng>>8)&1,(rng&1)?1:-1);
      if(t%step==0){report_mouse_t r=pointing_device_driver_get_report((report_mouse_t){0});
        if(select_button_pressed)printf("%d %d %d %d\n",step,t,r.h,r.v);}
    }
  }
}
