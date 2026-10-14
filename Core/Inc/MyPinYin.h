#ifndef __MY_PIN_YIN_H__
#define __MY_PIN_YIN_H__

//"========================================函数声明========================================"
//"拼音输入法杳询函数，input_py为已输入的拼音码，反回值为中文的起始地址，当为0时，杳询失败"
const char * py_ime(unsigned char input_py_val[]);

#endif


