#include <iostream>
#include <string>
#include <vector>
#include <mysql.h> 

using namespace std;

struct Transaccion {
    string tipo;   
    double monto;
    string correo; 
};

class CuentaBancaria {
private:
    int numeroCuenta;
    string nombreTitular;
    string tipoCuenta;
    double saldo;
    string correoElectronico; 
    vector<Transaccion> historial; 

public:
    CuentaBancaria(int num, string titular, string tipo, double saldoInicial, string correo) {
        numeroCuenta = num;
        nombreTitular = titular;
        tipoCuenta = tipo;
        saldo = saldoInicial;
        correoElectronico = correo;
    }

    void registrarEnBaseDatos(MYSQL* conectar) {
        string consulta = "INSERT INTO Cuentas (numero_cuenta, nombre_titular, saldo_actual, tipo_cuenta, correo_electronico) VALUES (" 
                          + to_string(numeroCuenta) + ", '" + nombreTitular + "', " + to_string(saldo) + ", '" + tipoCuenta + "', '" + correoElectronico + "') "
                          + "ON DUPLICATE KEY UPDATE saldo_actual = " + to_string(saldo) + ";";
        mysql_query(conectar, consulta.c_str());
    }

    void registrarMovimientoBD(MYSQL* conectar, string tipo, double monto, string destino = "NULL") {
        string consulta = "INSERT INTO Transacciones (numero_cuenta, tipo_movimiento, monto, cuenta_destino) VALUES ("
                          + to_string(numeroCuenta) + ", '" + tipo + "', " + to_string(monto) + ", " + destino + ");";
        mysql_query(conectar, consulta.c_str());
    }

    void mostrarDatos() {
        cout << "\n--- DATOS DE LA CUENTA ---" << endl;
        cout << "Titular: " << nombreTitular << " | Correo: " << correoElectronico << endl;
        cout << "Cuenta No: " << numeroCuenta << " | Tipo: " << tipoCuenta << endl;
        cout << "Saldo Disponible: Q" << saldo << endl;
    }

    void depositar(double monto, MYSQL* conectar) {
        if (monto > 0) {
            saldo += monto;
            Transaccion t = {"Deposito", monto, "N/A"};
            historial.push_back(t);
            cout << "¡Deposito exitoso!" << endl;
            registrarEnBaseDatos(conectar);
            registrarMovimientoBD(conectar, "Deposito", monto);
        }
    }

    void retirar(double monto, MYSQL* conectar) {
        double comision = 0.0;
        if (tipoCuenta == "Ahorro" || tipoCuenta == "ahorro") comision = monto * 0.02; 
        double totalDebitado = monto + comision;

        if (saldo >= totalDebitado) {
            saldo -= totalDebitado;
            cout << "Retiro exitoso de: Q" << monto << endl;
            Transaccion t = {"Retiro", monto, "N/A"};
            historial.push_back(t);
            registrarEnBaseDatos(conectar);
            registrarMovimientoBD(conectar, "Retiro", monto);
        } else {
            cout << "Error: Fondos insuficientes." << endl;
        }
    }

    void transferir(double monto, CuentaBancaria* cuentaDestino, string correoConfirmacion, MYSQL* conectar) {
        if (saldo >= monto) {
            saldo -= monto;
            cuentaDestino->saldo += monto; 

            Transaccion tOrigen = {"Transferencia Enviada", monto, correoConfirmacion};
            this->historial.push_back(tOrigen);

            Transaccion tDestino = {"Transferencia Recibida", monto, correoConfirmacion};
            cuentaDestino->historial.push_back(tDestino);

            cout << "¡Transferencia realizada con exito!" << endl;
            
            registrarEnBaseDatos(conectar);
            cuentaDestino->registrarEnBaseDatos(conectar);
            
            registrarMovimientoBD(conectar, "Transferencia Enviada", monto, to_string(cuentaDestino->getNumeroCuenta()));
            cuentaDestino->registrarMovimientoBD(conectar, "Transferencia Recibida", monto, "NULL");
        } else {
            cout << "Error: Fondos insuficientes." << endl;
        }
    }

    int getNumeroCuenta() { return numeroCuenta; }
    
    void mostrarHistorial() {
        cout << "\n--- HISTORIAL DE MOVIMIENTOS ---" << endl;
        for (size_t i = 0; i < historial.size(); i++) {
            cout << "- " << historial[i].tipo << ": Q" << historial[i].monto << endl;
        }
    }
};

int main() {
    MYSQL* conectar = mysql_init(NULL);
    if (!conectar) {
        cout << "Error al inicializar MySQL." << endl;
        return 1;
    }

    conectar = mysql_real_connect(conectar, "localhost", "root", "dani880@MX", "SistemaBancarioUMG", 3306, NULL, 0);

    if (!conectar) {
        cout << "Error critico de conexion a la base de datos: " << mysql_error(conectar) << endl;
        return 1;
    }

    vector<CuentaBancaria*> baseDatosCuentas;
    baseDatosCuentas.push_back(new CuentaBancaria(101, "Carlos Lopez", "Monetaria", 1500.00, "carlos@gmail.com"));
    baseDatosCuentas.push_back(new CuentaBancaria(202, "Maria Gomez", "Ahorro", 500.00, "maria@gmail.com"));

    int opcion = 0;
    while (opcion != 6) {
        cout << "   SISTEMA BANCARIO   " << endl;
        cout << "1. Registrar Cuenta Nueva" << endl;
        cout << "2. Realizar Deposito" << endl;
        cout << "3. Realizar Retiro" << endl;
        cout << "4. Realizar Transferencia" << endl;
        cout << "5. Consultar Datos e Historial" << endl;
        cout << "6. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                int num; string titular, tipo, correo; double saldoInicial;
                cout << "\n--- NUEVA CUENTA ---" << endl;
                cout << "Numero de Cuenta unico: "; cin >> num;
                cout << "Nombre del Titular: "; cin.ignore(); getline(cin, titular);
                cout << "Tipo (Monetaria/Ahorro): "; cin >> tipo;
                cout << "Saldo Inicial: Q"; cin >> saldoInicial;
                cout << "Correo Electronico: "; cin >> correo;

                CuentaBancaria* nueva = new CuentaBancaria(num, titular, tipo, saldoInicial, correo);
                nueva->registrarEnBaseDatos(conectar);
                baseDatosCuentas.push_back(nueva);
                cout << "¡Cuenta guardada en el sistema y en MySQL!" << endl;
                break;
            }
            case 2: {
                int num; double monto;
                cout << "Ingrese el numero de cuenta: "; cin >> num;
                for(auto c : baseDatosCuentas) {
                    if(c->getNumeroCuenta() == num) {
                        cout << "Monto a depositar: Q"; cin >> monto;
                        c->depositar(monto, conectar);
                    }
                }
                break;
            }
            case 3: {
                int num; double monto;
                cout << "Ingrese el numero de cuenta: "; cin >> num;
                for(auto c : baseDatosCuentas) {
                    if(c->getNumeroCuenta() == num) {
                        cout << "Monto a retirar: Q"; cin >> monto;
                        c->retirar(monto, conectar);
                    }
                }
                break;
            }
            case 4: {
                int origen, destino; double monto; string correo;
                cout << "Cuenta Origen: "; cin >> origen;
                cout << "Cuenta Destino: "; cin >> destino;
                cout << "Monto a transferir: Q"; cin >> monto;
                cout << "Correo de confirmacion: "; cin >> correo;

                CuentaBancaria* cOrigen = nullptr;
                CuentaBancaria* cDestino = nullptr;

                for(auto c : baseDatosCuentas) {
                    if(c->getNumeroCuenta() == origen) cOrigen = c;
                    if(c->getNumeroCuenta() == destino) cDestino = c;
                }

                if(cOrigen != nullptr && cDestino != nullptr) {
                    cOrigen->transferir(monto, cDestino, correo, conectar);
                } else {
                    cout << "Error: Cuentas invalidas." << endl;
                }
                break;
            }
            case 5: {
                int num;
                cout << "Ingrese el numero de cuenta a consultar: "; cin >> num;
                for(auto c : baseDatosCuentas) {
                    if(c->getNumeroCuenta() == num) {
                        c->mostrarDatos();
                        c->mostrarHistorial();
                    }
                }
                break;
            }
            case 6:
                cout << "\nCerrando conexion con el servidor MySQL..." << endl;
                break;
        }
    }

    for(auto c : baseDatosCuentas) delete c;
    mysql_close(conectar);
    return 0;
}