#ifndef MTYPES_H_INCLUDED
#define MTYPES_H_INCLUDED

// #define nvl_use_vclstring // разрешение использовать AnsiString из библиотеки VCL вместо std::string
 #define nvl_use_winapi  //  использовать WinAPI в некоторых местах вместо стандартной библиотеки Си


#include <string>       // для работы с std::string

#include <stdio.h>      // для работы с FILE
//#include <sys\stat.h> // для чтения размера файла
 #include <sys/stat.h>  // linux-стиль

#ifdef nvl_use_vclstring
typedef AnsiString MString;
#else
typedef std::string MString;
#endif


#ifdef nvl_use_winapi
    #include <windows.h>
    //#include <winnt.h>
    //#include <winuser.h>
#else
    #include <stdlib.h>
    #include <mem.h>     // для memset
#endif



class MyBString // analog of StringBuilder
{
  private:
  int size; // текущий размер буфера
  int last; // позиция записи куда БУДЕТ добавлен символ (текущий размер значащей части строки)
  int pos; // позиция чтения откуда будет считан следующий символ;
  char c; // последний прочитанный символ
  char* cstr; // собственно сама char-строка буфера

  public:
  int getCapacity() { return size; };
  char getLastReadedSymb() { return c; };
  int getWritePos() { return last; };
  int getReadPos() { return pos; };
  int setReadPos(int spos) { if(spos<last) pos = spos; }
  char* c_str() { return cstr; };


  // конструкция-деструкция:

  MyBString(int startsize)
     { cstr = NULL;  size = 0; Clear(startsize);
       #ifdef nvl_use_vcl
       DecimalSeparator = '.';
       #endif
     };

  MyBString()
     { cstr = NULL; size = 0; Clear(256);
        #ifdef nvl_use_vcl
        DecimalSeparator = '.';
        #endif
	 };

  ~MyBString() { delete cstr; };


  // методы изменяющие размер буфера:

  // установка нового размера строки и заполнение края нулями:
  void grow(int snewsize)
  {
    char* ncs = new char[snewsize];
    memset(ncs, '\0',snewsize);         // заполнение нулями;
    if(cstr){ memcpy(ncs, cstr, size);  // копирование в новую из старой
              delete cstr;              // удаление старой;
            };
    cstr = ncs;
    size= snewsize;
  };

  // увеличение позиции записи с учётом изменения ёмкости:
  int IncLast(int l=1)
  {
    last+=l;
    if(last >= size)
          { int nsize = size*2; // +k;
            while (last >= nsize) nsize*=2; //  +=k;
            grow(nsize);    // str.SetLength(size);
          };
    return last;
  };

  // стирание и инициализация нулями:
  void Clear(int startsize=0)
   {
     if(startsize==0) startsize=size;

     last = 0;  pos= 0; // -1;
     if(startsize!=size)
     { if(cstr) delete cstr;
        size = startsize;
        cstr = new char[startsize]; // str.SetLength(k);
     };
     memset(cstr, '\0', size);
   };


  // МЕТОДЫ ЗАПИСИ В ПОТОК:

  // запись одного символа. базовое действие:
  MyBString* operator+=(char c)
     {
       cstr[last]=c; IncLast();
       return this;
     };

  #ifdef nvl_use_vcl
  // запись строки (копированием фрагмента памяти):
  MyBString* operator+=(AnsiString s)
     { int l = s.Length(); if(l==0) return this; // это важно!
       int ll = last;
       IncLast(l);
       memcpy(cstr+ll, s.c_str(), l); // for(int i=1; i<=l; i++) str[ll+i]=s[i];
       return this;
     };
  #endif

  MyBString* operator+=(std::string s)
     { int l = s.length();
       int ll = last;
       IncLast(l);
       memcpy(cstr+ll, s.c_str(), l); // for(int i=1; i<=l; i++) str[ll+i]=s[i];
       return this;
     };

  MyBString* operator+=(MyBString bs)
     { int l = bs.last; //s.length();
       int ll = last;
       IncLast(l);
       memcpy(cstr+ll, bs.cstr, l); // for(int i=1; i<=l; i++) str[ll+i]=s[i];
       cstr[last] = '\0'; // перестраховка, вообще 0 там ещё из memset в grow
       return this;
     };

  MyBString* operator+=(char* str)
     {
       int l = strlen(str); // 0; char c = str[0]; while(c!='\0') { l++; c=str[l]; };
       int ll = last;
       IncLast(l);
       memcpy(cstr+ll, str, l); // for(int i=1; i<=l; i++) str[ll+i]=s[i];
       return this;
     };

  // метод записи заковыченной строки с явным указанием кавычки:
  void WriteQuotedString(MString str, bool quoted=1, char quote='\'')
     {
        if(quoted) this->operator+=(quote);  // открывающая кавычка
        const char* c_str = str.c_str(); char c = ' ';
        for(int i=0; ;i++)
        { c = c_str[i];
          if(c=='\0') break; // конец строки
          if(c!='\r')  //игнорируем 0D, оставляем 0A
          this->operator+=(c); // добавляем прочитанный символ
          if(c==quote)this->operator+=(c);   // экранируем внутренние кавычки
        };
        if(quoted) this->operator+=(quote);  // закрывающая кавычка
     };


  // МЕТОДЫ ЧТЕНИЯ ИЗ ПОТОКА:

   // экстракторы вынесены наружу, как глобальные функции, а не методы класса
  friend void operator>>(MyBString& mb, char& a);
  #ifdef nvl_use_vcl
  friend void operator>>(MyBString& mb, AnsiString& string);
  #endif
  friend void operator>>(MyBString& mb, std::string& string);
  friend void operator>>(MyBString& mb, int& a);
  friend void operator>>(MyBString& mb, float& a);
  friend void operator>>(MyBString& mb, bool& a);

  // чтение токенов:
  int ReadDigChars(char* lex, int maxsize=64); // читает токен в char* строку
  int ReadToken(MyBString &bstr); // читает токен в bstr
  int getToken(MyBString &bstr); // синоним ReadToken

  MString ReadQuotedString(); // метод чтения заковыченной строки с любой кавычки

  enum tchr   // типы символов для разбора токенов:
      {  tcNormal = 0,  // обычный символ
         tcDelimiter,   // разделитель
         tcQuote,       // кавычка
         tcSpacer       // пробельный символ
      };
  inline tchr tc(char c); // функция возвращает тип символа для токенов:

  // служебные, возможно даже private используемые при чтении токенов:
  inline int ReadQuotedChars(char* lex, char quote, int maxsize); // используется для чисел
  inline int ReadQuotedMyBString(MyBString &bstr, char quote); // используется для строк
  inline char ReadTail(); // увеличивает pos пока не наткнётся на разделитель или значащий символ


  // чтение строки как есть от текущей позиции до символа перевода строки:
  #ifdef nvl_use_vcl
  int getLine(AnsiString &str);
  #endif
  int getLine(std::string &str);
  int getLine(MyBString &bstr);

  int findEndl(); // сдвигает pos до ближайшего перевода строки, возвращает длину

  // получение всей строки буфера как MString-а
  MString asMString() { return MString(cstr); };


  // РАБОТА С ФАЙЛОМ:
  bool LoadFromFile(MString FileName)
    {
      FILE *stream = fopen(FileName.c_str(),"rb");
      if(!stream) return 0;
      struct stat fileStat; stat(FileName.c_str(), &fileStat);
      size = fileStat.st_size;

      if(cstr) delete cstr;
      cstr = new char[size+1];

      fread(cstr, size, 1, stream);
      fclose(stream);

      cstr[size]='\0'; // последним символом должен быть нолик
      last = size;     // последний записанный логично в конце загруженного блока
      size++;	       // настоящий размер на один больше

      pos=0;
      return 1;
    };

  bool SaveToFile(MString FileName)
  {
   try
   {
    FILE *stream = fopen(FileName.c_str(), "wb");
    if(!stream) return 0;

    fwrite(cstr, last, 1, stream);
    fclose(stream);
    return 1;
   }
   catch (...)
   {
    return 0;
   };
  };

  // для совместимости с макросами строк:
  int Length() { return last; };
  int length() { return last; };

  // эмуляция потоков:
  bool eof() { if(pos>=last) return 1; else return 0; };
  bool close() { return 1; };
  bool get(char& c) // {c=cstr[pos]; pos++; if(pos>=lust) { c='\0'; return 0; }; ; return 1; };
                        { if(pos>=last) { c='\0'; return 0; }; c=cstr[pos]; pos++; return 1; };

};


// ОПЕРАТОРЫ ЧТЕНИЯ ИЗ ПОТОКА:

 void operator>>(MyBString& mb, char& a);

 void operator>>(MyBString& mb, int& a);

 void operator>>(MyBString& mb, __int64& a);

 void operator>>(MyBString& mb, float& a);

 void operator>>(MyBString &mb, bool& b);


// ОПЕРАТОРЫ ЗАПИСИ В ПОТОК:

 MyBString& operator<<(MyBString &mb, char c);

 MyBString& operator<<(MyBString &mb, char* str);

 #ifdef nvl_use_vcl
 MyBString& operator<<(MyBString &mb, AnsiString s);
 #endif

 MyBString& operator<<(MyBString &mb, std::string s);

 MyBString& operator<<(MyBString &mb, int i);

 MyBString& operator<<(MyBString &mb, __int64 i);

 MyBString& operator<<(MyBString &mb, float i);

 MyBString& operator<<(MyBString &mb, bool i);

// Комментарий к перегрузке операций записи типа operator<<(MyBString &mb, int i)
// Эти штуки нужны, чтобы у компилятора не возникал конфликт. Дело в том, что когда
// он видит что-то типа mb<<4, он не знает что нужно делать - вызывать ли базовый метод
// для записи числа в поток, либо перекрытый, для записи в поток String-а,
// предварительно 4-ку в этот самый String преобразовав.


 // Неоднозначные операции:
 // Чтение в строку неоднозначно, т.к может означать чтение токена, а может - просто строки
 // до ближайшего перевода строки. Потому во избежание неприятностей лучше не использовать
 // этот экстрактор, а явно вызывать getToken или getLine
 #ifdef nvl_use_vcl
 void operator>>(MyBString& mb, AnsiString& str);
 #endif
 void operator>>(MyBString& mb, std::string& str);


#endif // MTYPES_H_INCLUDED
