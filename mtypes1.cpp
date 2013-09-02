#include "mtypes1.h"

// ОПЕРАТОРЫ ЧТЕНИЯ ИЗ ПОТОКА (инсерторы):

// чтение одного символа, самая простая функция:
void operator>>(MyBString& mb, char& a)
  {  if(mb.pos<mb.last) a=mb.cstr[mb.pos];
     else a='\0';
     mb.pos++;
  };



// обращаю внимание - метод возвращает строку без кавычек по краям, убранными экранами,
// с '\0' в конце; причём в c остаётся символ следующий после закрывающей кавычки;
// причём считается что вначале позиция чтения находится уже после открывающей кавычки
int MyBString::ReadQuotedChars(char* lex, char quote, int maxsize)
{
  int dx=0;
  while(get(c))  // читаем всё внутрь строки
  {
     if(c==quote){ if(cstr[pos]==quote) pos++; // экран
                   else break; // а если не экран - то закончили
                 };
     if(c=='\r') continue; // все 0D-символы просто пропускаем.
     if(c=='\n') { lex[dx]='\r'; dx++; }; //b+="\r\n"; // это идиотизм для AnsiString-ов
     lex[dx]=c; dx++; // в нормальных случаях добавляем прочитанное
     if(dx>maxsize) break;
  };

 lex[dx]='\0';
 return dx;
};


// аналогичный но более удобный метод, т.к. буфер для чтения расширяем
int MyBString::ReadQuotedMyBString(MyBString &bstr, char quote)
{
  int spos = bstr.pos;
  while(get(c))  // читаем всё внутрь строки
  {
     if(c==quote)//{ if(!get(c)) break; // если кавычка это конец cstr - выход
                 //  if(c!=quote) break; // но если не встретим экранирующей кавычки
                 //};
                 { if(cstr[pos]==quote) pos++; // экран
                   else break; // а если не экран - то закончили
                 };
     if(c=='\r') continue; // все 0D-символы просто пропускаем.
     if(c=='\n') bstr+='\r'; //b+="\r\n"; // это идиотизм для AnsiString-ов
     bstr+=c; // в нормальных случаях добавляем прочитанное
  };

 bstr.cstr[bstr.last] = '\0'; // перестраховка
 return (bstr.pos - spos); // вернём количество записанных в bstr байт
};


int MyBString::ReadDigChars(char* str, int maxsize)
{
 int dx=0;  if(!str) return 0;

 while(get(c))
 switch(tc(c))
 {
   // если терминальные символы найдены раньше цифровых - просто выходим:
   case tcDelimiter:  str[dx]='\0'; return dx;  // break;

   // символы форматирования - пробелы, переносы, табуляции - пропускаются:
   case tcSpacer:  continue;  //break;

   // для чтения чисел в кавычках - кавычки отбрасываются!
   case tcQuote: dx+= ReadQuotedChars(&str[dx], c, maxsize-dx); goto lread;  //break;

   // во всех остальных случаях - читаем лексему:
   default: str[dx]=c; dx++; goto lread;  //break;
 };

 lread:
 while(get(c)) // дочитываем остальные символы
 switch(tc(c))
 {
   // обращаю внимание, что когда число уже началось, пробелы и переносы считаются разделителями:
   // но после них нужно дочитать хвост пробелов
   case tcSpacer:  ReadTail();  str[dx]='\0'; return dx; //break;

   case tcDelimiter: str[dx]='\0';  return dx; //break;

   default: str[dx]=c; dx++; if(dx>maxsize) { str[dx]=0; return c; }; break;
 }

 str[dx]='\0';
 return dx;
};


int MyBString::ReadToken(MyBString &bstr) // читает токен в bstr
{
 bstr.last = 0; // clear не делаю, т.к. она делает memset на полный размер буфера
 while(get(c))
 switch(tc(c))
 {
   // если терминальные символы найдены раньше цифровых - просто выходим:
   case tcDelimiter:  bstr.cstr[bstr.last] = '\0'; return bstr.last;  //break;

   // символы форматирования - пробелы, переносы, табуляции - пропускаются
   case tcSpacer: if(bstr.last==0) continue; else  // пока токен ещё пуст
          ReadTail(); bstr.cstr[bstr.last] = '\0'; return bstr.last;
   //break;

   // для чтения чисел в кавычках - кавычки отбрасываются!
   case tcQuote:
         //в этом месте может быть анализ предкавычного префикса вроде L"abc"
         this->ReadQuotedMyBString(bstr,c);
         // сейчас в c кавычка, потому нужно переходить к следующему символу
         // кстати, при желании строку можно и вернуть в кавычках из c
   break;

   // во всех остальных случаях - читаем лексему:
   default: bstr+=c; break;
 };

 bstr.cstr[bstr.last] = '\0'; // перестраховка
 return bstr.last;
};


char MyBString::ReadTail() // увеличивает pos пока не наткнётся на разделитель или значащий символ
{
 while(pos<last)
 {
   char nc = cstr[pos];
   switch(tc(nc))
    {
      case tcDelimiter:   c = nc; pos++; return c;  //break;

      // c = nc;  // пробельные пропускаем чтобы в c был последний разделитель
      case tcSpacer:      pos++; continue;  //break;

      default: // все остальные символы считаются значимыми и это уже новый токен
         return c; // а разделителем выступает последний пробельный символ перед ним
      //break;
    };
  };
  return '\0';
};


MyBString::tchr MyBString::tc(char c)
{
 switch(c)
 {
   case '\'': case '"':
        return tcQuote;

   case ' ': case '\t': case '\r':  case '\n':
         return tcSpacer;

   case '<': case '>': case '\0':
             case ':': case ';': case ',':  case '=':
             case '{': case '}': case '(': case ')': case '[': case ']':
         return tcDelimiter;

   default:
         return tcNormal;
 };
};


int MyBString::findEndl()
{
   if(pos>last) return 0;  // защита от дурака
   int spos = pos; c=' ';
   while((c!='\n')&&(c!='\r')&&(c!='\0'))
     { c = cstr[pos];
       pos++;
       if(pos>last) break; // была ошибка, не читался последний символ
     };
   return (pos - spos - 1); // последний считанный символ мы учитывать не должны
};


  #ifdef nvl_use_vcl
int MyBString::getLine(AnsiString &str)
{
   int spos = pos; int l = findEndl();
   str = AnsiString(cstr+spos, l);
   return l;
};
#endif

int MyBString::getLine(std::string &str)
{
   int spos = pos; int l = findEndl();
   str = std::string(cstr+spos, l);
   return l;
};

int MyBString::getLine(MyBString &bstr)
{
  int spos = pos; int l = findEndl();
  bstr.last = 0; bstr.IncLast(l);
  memcpy(bstr.cstr, cstr+spos, l);
  bstr.cstr[l] = '\0';
  return l;
};


// чтение int-числа
void operator>>(MyBString& mb, int& a)
          {
             char d[64];
             mb.ReadDigChars(d,64);
             a = atoi(d);        //-< a = as.ToInt();
           };

// чтение int64-числа

void operator>>(MyBString& mb, __int64& a)
          {
             char d[64];
             mb.ReadDigChars(d,64);

              #ifdef nvl_use_winapi
              a = _atoi64(d);
              #else
                #ifdef nvl_use_qt
                  QByteArray as = "";
                  as = d;
                  a = as.toLongLong();
                #else
                  // здесь должна быть чисто C-шная реализация
                  a = 0;
                #endif
              #endif
           };


// чтение float-числа
void operator>>(MyBString& mb, float& a)
           {
             char d[64];
             mb.ReadDigChars(d,64);
             a = atof(d);
             //a = StrToFloat(str.SubString(startpos, pos - startpos));
             //-< a = as.ToDouble();
           };

// чтение bool как int
void operator>>(MyBString &mb, bool& b)
          { int i; mb>>(i);
            b = i; return;// 1;
           };


// ОПЕРАТОРЫ ЗАПИСИ В ПОТОК (экстракторы):

// запись одного символа в поток - то же самое, что и +=
MyBString& operator<<(MyBString &mb, char c)
             {  //((std::strstream&)mb)<<c;
                mb.operator+=(c);
                return mb;
             };

#ifdef nvl_use_vcl
// запись строки, просто перевызов оператора вставки строки +=
MyBString& operator<<(MyBString &mb, AnsiString s)
             { // ((std::strstream&)mb)<< s.c_str();
               mb.operator+=(s);
               return mb;
             };
#endif

MyBString& operator<<(MyBString &mb, std::string s)
             { // ((std::strstream&)mb)<< s.c_str();
               mb.operator+=(s);
               return mb;
             };

// запись char-строки в поток
MyBString& operator<<(MyBString &mb, char* str)
             {
               mb.operator+=(str);
               return mb;
             };

// запись int-числа в поток
MyBString& operator<<(MyBString &mb, int i)
             {
               //mb.operator+=(IntToStr(i));

               char d[64];   // надеюсь, целых чисел больше 64 позиций не бывает
               itoa(i, d, 10);
               mb<<d;

               return mb;
             };

// запись int64-числа в поток
MyBString& operator<<(MyBString &mb, __int64 i)
             {
               //mb.operator+=(IntToStr(i));
#ifdef nvl_use_winapi
               char d[64];   // надеюсь, целых чисел больше 64 позиций не бывает
               _i64toa(i, d, 10);
               mb<<d;
#else
    #ifdef nvl_use_qt
               QByteArray qb = QByteArray::number(i);
               mb<<qb.data();
    #else
               mb<<0;  // не знаю я как int64 писать на чистом Си
    #endif
#endif
               return mb;
             };


// запись float-числа в поток
MyBString& operator<<(MyBString &mb, float f)
             {
               //mb.operator+=(FloatToStr(f));
               char d[64];
               sprintf(d, "%f", f);
               mb<<d;
               return mb;
             };

// запись bool в поток как int
MyBString& operator<<(MyBString &mb, bool i)
             {
               //((std::strstream&)mb)<<i;
               //mb.operator+=(IntToStr(i));
               int it = i;
               mb<<it;
               return mb;
             };



// Экстракторы строк, не рекомендуются к активному использованию,
// токены всегда лучше читать явным readToken

#ifdef nvl_use_vcl
void operator>>(MyBString& mb, AnsiString& str)
 {
    MyBString t(64);
    mb.ReadToken(t);
    str = AnsiString(t.cstr);  //str = t.cstr; // ужасно некрасиво
 }
 #endif

void  operator>>(MyBString& mb, std::string& str)
 {
    MyBString t(64);
    mb.ReadToken(t);
    //str = std::string(t.cstr); // ужасно некрасиво
    str = t.cstr;
 }
