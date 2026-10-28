#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include TARGET_TRACKBALL
uint16_t mock_now;
static unsigned checks;
static void reset_all(int precision) {
  pointer_reset();memset(rate_meters,0,sizeof rate_meters);
  memset(gliders,0,sizeof gliders);memset(wheel_buffer,0,sizeof wheel_buffer);
  last_mode=MODE_MOUSE;last_report=0;mock_now=0;
  pointer_reset_pending=false;
  select_button_pressed=false;select_button_scrolled=false;precision_mode=precision;
}
static report_mouse_t poll(int t) {
  mock_now=(uint16_t)t;
  report_mouse_t r=pointing_device_driver_get_report((report_mouse_t){0});
  assert(r.x>=-127&&r.y>=-127);checks++;
  return r;
}
static void expect(int condition) {assert(condition);checks++;}
static int expected_counts(int edges,int precision){return precision?edges*2:(int)floor((double)edges*12.5);}
static void isolated(void) {
  for(int p=0;p<2;p++)for(int step=1;step<=50;step++)for(int offset=0;offset<step;offset++){
    reset_all(p);int sum=0,first=-1;const int start=2000+offset;
    for(int t=0;t<start+1200;t++){
      mock_now=t;if(t==start)trackball_move(AXIS_X,1);
      if(t%step==0){report_mouse_t r=poll(t);sum+=r.x;if(r.x&&first<0)first=t-start;}
    }
    expect(sum==expected_counts(1,p));expect(first>=0&&first<step);
  }
}
static void burst(void){
  for(int p=0;p<2;p++)for(int sign=-1;sign<=1;sign+=2){
    reset_all(p);mock_now=2000;
    for(int i=0;i<2000;i++)trackball_move(AXIS_X,sign);
    int total=0;for(int i=0;i<300;i++)total+=poll(2000+i).x;
    expect(total==expected_counts(2000*sign,p));expect(distances[AXIS_X]==0);
    expect(poll(2400).x==0);
  }
}
static void diagonal_and_reverse(void){
  for(int p=0;p<2;p++){
    reset_all(p);mock_now=2000;trackball_move(AXIS_X,1);trackball_move(AXIS_Y,-1);
    report_mouse_t a=poll(2001);expect(a.x==expected_counts(1,p)&&a.y==expected_counts(-1,p));
    trackball_move(AXIS_X,-1);trackball_move(AXIS_Y,1);
    report_mouse_t b=poll(2002);expect(b.x==-a.x&&b.y==-a.y);
    b=poll(2050);expect(b.x==0&&b.y==0);
  }
}
static void random_counts(void){
  for(int p=0;p<2;p++)for(int step=1;step<=50;step++){
    reset_all(p);unsigned rng=12345;int expected[2]={0},actual[2]={0};
    for(int t=0;t<10000;t++){
      mock_now=t;rng=rng*1664525u+1013904223u;
      if((rng>>24)<100){int axis=(rng>>8)&1;int sign=(rng&1)?1:-1;
        trackball_move(axis,sign);expected[axis]+=sign;}
      if(t%step==0){report_mouse_t r=poll(t);actual[0]+=r.x;actual[1]+=r.y;}
    }
    report_mouse_t r;for(int i=0;i<100;i++){r=poll(10000+i);actual[0]+=r.x;actual[1]+=r.y;}
    expect(actual[0]==expected_counts(expected[0],p));expect(actual[1]==expected_counts(expected[1],p));
    r=poll(10100);expect(r.x==0&&r.y==0);
  }
}
static void saturation_and_modes(void){
  reset_all(0);mock_now=2000;distances[0]=INT32_MAX;trackball_move(0,1);expect(distances[0]==INT32_MAX);
  distances[0]=INT32_MIN;trackball_move(0,-1);expect(distances[0]==INT32_MIN);
  reset_all(0);select_button_pressed=true;poll(2000);mock_now=2001;trackball_move(0,1);
  select_button_pressed=false;report_mouse_t r=poll(2002);expect(r.x==0&&r.y==0&&r.h==0&&r.v==0);
  mock_now=2003;trackball_move(0,1);r=poll(2004);expect(r.x==12);
}
static void suspend_resume(void){
  reset_all(0);mock_now=2000;trackball_move(0,1);
  suspend_power_down_kb();
  for(int i=0;i<500;i++){mock_now++;trackball_move(0,1);trackball_move(1,-1);}
  suspend_wakeup_init_kb();
  report_mouse_t r=poll(3000);expect(r.x==0&&r.y==0);
  mock_now=3001;trackball_move(0,1);r=poll(3002);expect(r.x==12);
  // A wakeup hook alone must discard any unreported pre-wakeup counts.
  trackball_move(0,1);suspend_wakeup_init_kb();r=poll(3003);expect(r.x==0);
}
static void constant_gain_every_speed_and_length(void){
  static const int intervals[]={0,1,5,10,20,25,49,50,100,500};
  static const int lengths[]={1,3,10,30,100};
  for(int p=0;p<2;p++)for(int sign=-1;sign<=1;sign+=2)
  for(unsigned d=0;d<sizeof intervals/sizeof *intervals;d++)
  for(unsigned n=0;n<sizeof lengths/sizeof *lengths;n++)
  for(int cadence=1;cadence<=50;cadence++)for(int order=0;order<2;order++){
    reset_all(p);int totals[2]={0};
    for(int i=0;i<lengths[n];i++){
      mock_now=(uint16_t)(1000+i*intervals[d]);
      if(order&&i%2==0)trackball_move(1,-sign);
      trackball_move(0,sign);
      if(!order&&i%2==0)trackball_move(1,-sign);
      if(i%cadence==0){report_mouse_t r=poll(mock_now);totals[0]+=r.x;totals[1]+=r.y;}
    }
    for(int i=0;i<100;i++){report_mouse_t r=poll(mock_now+1);totals[0]+=r.x;totals[1]+=r.y;}
    expect(totals[0]==expected_counts(lengths[n]*sign,p));
    expect(totals[1]==expected_counts(-((lengths[n]+1)/2)*sign,p));
    expect(distances[0]==0&&distances[1]==0);
  }
}
static void exact_report_carry(void){
  reset_all(0);mock_now=1000;
  for(int i=0;i<13;i++)trackball_move(0,1);
  expect(poll(1001).x==125);expect(distances[0]==3);
  expect(poll(1002).x==37);expect(pointer_half_remainder[0]==1);
  expect(poll(1003).x==0);
  mock_now=1004;trackball_move(0,-1);expect(poll(1005).x==-12);
  expect(pointer_half_remainder[0]==0);
}
static void fractional_remainder_and_precision(void){
  reset_all(0);mock_now=1000;trackball_move(0,1);expect(poll(1000).x==12);
  expect(pointer_half_remainder[0]==1);
  for(int i=0;i<1000;i++){expect(poll(1001+i).x==0);expect(pointer_half_remainder[0]==1);}
  mock_now=3000;trackball_move(0,1);expect(poll(3000).x==13);expect(pointer_half_remainder[0]==0);
  trackball_move(0,-1);expect(poll(3001).x==-13);expect(pointer_half_remainder[0]==1);
  trackball_move(0,-1);expect(poll(3002).x==-12);expect(pointer_half_remainder[0]==0);
  // Precision always emits exactly2, without disturbing normal half-counts.
  trackball_move(0,1);expect(poll(3003).x==12);precision_mode=true;
  for(int sign=-1;sign<=1;sign+=2){trackball_move(0,sign);expect(poll(3004).x==sign*2);expect(pointer_half_remainder[0]==1);}
  precision_mode=false;trackball_move(0,1);expect(poll(3005).x==13);expect(pointer_half_remainder[0]==0);
  // Deliberate mode/suspend resets clear the raw queue and half-count together.
  trackball_move(0,1);expect(poll(3006).x==12);expect(pointer_half_remainder[0]==1);
  select_button_pressed=true;poll(3007);expect(pointer_half_remainder[0]==0);
  select_button_pressed=false;poll(3008);trackball_move(0,1);expect(poll(3009).x==12);
  suspend_wakeup_init_kb();expect(poll(3010).x==0);expect(pointer_half_remainder[0]==0);
}
static void exhaustive_reversal_grouping(void){
  // Compare every +/- sequence of 1..8 edges at all report-boundary masks.
  for(int length=1;length<=8;length++)
  for(unsigned sequence=0;sequence<(1U<<length);sequence++)for(unsigned boundaries=0;boundaries<(1U<<length);boundaries++){
    reset_all(0);int net=0,out=0;
    for(int i=0;i<length;i++){
      mock_now=1000+i;int sign=(sequence&(1U<<i))?1:-1;net+=sign;trackball_move(0,sign);
      if(boundaries&(1U<<i))out+=poll(mock_now).x;
    }
    out+=poll(1010).x;
    expect(out==expected_counts(net,0));
    expect(2*out+pointer_half_remainder[0]==25*net);
    expect(pointer_half_remainder[0]<=1);expect(distances[0]==0);
  }
  // Explicit odd-total regression: +3 then -2 and batched +1 both emit12.
  for(int split=0;split<2;split++){
    reset_all(0);int out=0;
    for(int i=0;i<3;i++)trackball_move(0,1);
    if(split)out+=poll(1000).x;
    for(int i=0;i<2;i++)trackball_move(0,-1);
    out+=poll(1001).x;expect(out==12);expect(pointer_half_remainder[0]==1);
  }
}
int main(void){isolated();burst();diagonal_and_reverse();random_counts();saturation_and_modes();suspend_resume();constant_gain_every_speed_and_length();exact_report_carry();fractional_remainder_and_precision();exhaustive_reversal_grouping();printf("PASS %u checks: fixed 12.5 gain, fractional carry/idle, cadence+phase+reversal grouping, exact precision2, varied stroke lengths/speeds, bursts, diagonals/order, conservation, overflow, no drift, saturation, mode and suspend resets\n",checks);}
