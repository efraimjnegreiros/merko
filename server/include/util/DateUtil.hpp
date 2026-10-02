#ifndef DATE_UTIL_HPP
#define DATE_UTIL_HPP

#include <string>
#include <ctime>

using namespace std;

// funcoes de data, tudo no formato AAAA-MM-DD q é como fica salvo no banco
namespace DateUtil {

    string hoje(); // data de hoje

    time_t paraTimeT(const string& dataIso); // string pra time_t

    string paraString(time_t t); // time_t pra string

    int diferencaEmDias(const string& data1, const string& data2); // data2 - data1 em dias

    string somarMeses(const string& dataIso, int meses); // soma meses numa data
}

#endif
