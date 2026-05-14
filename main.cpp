// ============================================================
//  Descripcion:
//  Sistema de gestion para una empresa Lotificadora.
//  Permite administrar Proyectos y sus Lotes desde consola,
//  conectandose a una base de datos MySQL local mediante
//  el framework EloquentORM hecho en C++.
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include "MySQLConexion.h"
#include "EloquentORM.h"

using namespace std;

// ============================================================
//  BLOQUE 1: COLORES PARA LA CONSOLA
// ============================================================
const string RESET    = "\033[0m";
const string VERDE    = "\033[32m";
const string ROJO     = "\033[31m";
const string AMARILLO = "\033[33m";
const string CYAN     = "\033[36m";
const string BLANCO   = "\033[97m";

// ============================================================
//  BLOQUE 2: FUNCIONES AUXILIARES
// ============================================================
void limpiar() { system("cls"); }
void pausa() { system("pause"); }
void linea() { cout << "======================================" << endl; }
void msg_exito(const string& texto) { cout << VERDE << "[+] " << texto << RESET << endl; }
void msg_error(const string& texto) { cout << ROJO << "[!] " << texto << RESET << endl; }
void msg_aviso(const string& texto) { cout << AMARILLO << "[~] " << texto << RESET << endl; }

int leer_entero(const string& prompt) {
    int valor;
    cout << prompt;
    while (!(cin >> valor)) {
        cin.clear();
        cin.ignore(1000, '\n');
        msg_error("Solo se aceptan numeros. Intente de nuevo.");
        cout << prompt;
    }
    cin.ignore();
    return valor;
}

// ============================================================
//  BLOQUE 3: CLASE PROYECTO
// ============================================================
class Proyecto {
public:
    string nombreProyecto;
    string ubicacionProyecto;

    void registrar_nuevo_proyecto(EloquentORM& orm) {
        limpiar();
        cout << CYAN; linea();
        cout << "     REGISTRAR NUEVO PROYECTO" << endl;
        linea(); cout << RESET;

        cout << "Nombre del Proyecto : "; getline(cin, nombreProyecto);
        cout << "Ubicacion           : "; getline(cin, ubicacionProyecto);

        orm.set("nombre_proyecto", nombreProyecto);
        orm.set("ubicacion",       ubicacionProyecto);

        if (orm.save()) {
            msg_exito("Proyecto '" + nombreProyecto + "' guardado con exito.");
        } else {
            msg_error("No se pudo guardar. Verifique la conexion.");
        }
    }

    static void mostrar_todos(EloquentORM& orm) {
        auto lista = orm.getAll();
        cout << AMARILLO; linea();
        cout << "       LISTADO DE PROYECTOS" << endl;
        linea(); cout << RESET;

        if (lista.empty()) {
            msg_aviso("No hay proyectos registrados en el sistema.");
            return;
        }

        for (auto& fila : lista) {
            cout << "  ID: "        << BLANCO << fila["id"]                << RESET
                 << " | Nombre: "  << BLANCO << fila["nombre_proyecto"]  << RESET
                 << " | Lugar: "   << BLANCO << fila["ubicacion"]        << RESET
                 << endl;
        }
        linea();
    }

    void actualizar_proyecto(EloquentORM& orm) {
        limpiar();
        cout << CYAN << "      EDITAR PROYECTO" << RESET << endl;
        mostrar_todos(orm);

        string idStr = to_string(leer_entero("\nID del proyecto a editar: "));
        cout << "Nuevo nombre    : "; getline(cin, nombreProyecto);
        cout << "Nueva ubicacion : "; getline(cin, ubicacionProyecto);

        orm.set("id", idStr); 
        orm.set("nombre_proyecto", nombreProyecto);
        orm.set("ubicacion", ubicacionProyecto);

        if (orm.update()) {
            msg_exito("Proyecto actualizado correctamente.");
        } else {
            msg_error("Algo salio mal al actualizar. Verifique el ID.");
        }
    }

    void eliminar_proyecto(EloquentORM& orm) {
        limpiar();
        cout << ROJO << "      ELIMINAR PROYECTO" << RESET << endl;
        mostrar_todos(orm);
        string idStr = to_string(leer_entero("\nID del proyecto a eliminar: "));
        
        orm.set("id", idStr); 

        if (orm.remove()) {
            msg_exito("Proyecto eliminado del sistema.");
        } else {
            msg_error("No se pudo eliminar.");
        }
    }
};

// ============================================================
//  BLOQUE 4: CLASE LOTE
// ============================================================
class Lote {
public:
    string numeroLote;
    string precioLote;
    string estadoLote;
    string proyectoId;

    string pedir_estado() {
        cout << "  Estado del lote:" << endl;
        cout << "    1. Disponible" << endl;
        cout << "    2. Vendido"    << endl;
        cout << "    3. Reservado"  << endl;
        int opEstado = leer_entero("  Seleccione estado: ");

        switch (opEstado) {
            case 2:  return "vendido";
            case 3:  return "reservado";
            default: return "disponible";
        }
    }

    void registrar_nuevo_lote(EloquentORM& ormLote, EloquentORM& ormProyecto) {
        limpiar();
        cout << CYAN; linea();
        cout << "       REGISTRAR NUEVO LOTE" << endl;
        linea(); cout << RESET;

        cout << "\n-- Proyectos disponibles --" << endl;
        Proyecto::mostrar_todos(ormProyecto);

        proyectoId = to_string(leer_entero("Asociar a Proyecto ID: "));
        cout << "Numero de lote (ej: L-001) : "; getline(cin, numeroLote);
        cout << "Precio del lote (Q)        : "; getline(cin, precioLote);
        estadoLote = pedir_estado();

        ormLote.set("numero_lote",  numeroLote);
        ormLote.set("precio",       precioLote);
        ormLote.set("estado",       estadoLote);
        ormLote.set("proyecto_id",  proyectoId);

        if (ormLote.save()) {
            msg_exito("Lote '" + numeroLote + "' registrado con exito.");
        } else {
            msg_error("No se pudo guardar el lote.");
        }
    }

    static void mostrar_todos(EloquentORM& orm) {
        auto lista = orm.getAll();
        cout << AMARILLO; linea();
        cout << "        LISTADO DE LOTES" << endl;
        linea(); cout << RESET;

        if (lista.empty()) {
            msg_aviso("No hay lotes registrados en el sistema.");
            return;
        }

        for (auto& fila : lista) {
            string colorEstado = BLANCO;
            if (fila["estado"] == "disponible") colorEstado = VERDE;
            if (fila["estado"] == "vendido")    colorEstado = ROJO;
            if (fila["estado"] == "reservado")  colorEstado = AMARILLO;

            cout << "  ID: "           << BLANCO      << fila["id"]          << RESET
                 << " | Lote: "        << BLANCO      << fila["numero_lote"] << RESET
                 << " | Precio: Q"     << BLANCO      << fila["precio"]      << RESET
                 << " | Estado: "      << colorEstado << fila["estado"]      << RESET
                 << " | Proyecto ID: " << BLANCO      << fila["proyecto_id"] << RESET
                 << endl;
        }
        linea();
    }

    void actualizar_lote(EloquentORM& orm) {
        limpiar();
        cout << CYAN; linea();
        cout << "        EDITAR LOTE" << endl;
        linea(); cout << RESET;

        mostrar_todos(orm);
        string idStr = to_string(leer_entero("\nID del lote a editar: "));

        cout << "\n  Que desea actualizar?" << endl;
        cout << "    1. Solo el precio"    << endl;
        cout << "    2. Solo el estado"    << endl;
        cout << "    3. Precio y estado"   << endl;
        int opcion = leer_entero("  Seleccione: ");

        orm.set("id", idStr); 

        if (opcion == 1 || opcion == 3) {
            cout << "Nuevo precio (Q): "; getline(cin, precioLote);
            orm.set("precio", precioLote);
        }
        if (opcion == 2 || opcion == 3) {
            estadoLote = pedir_estado();
            orm.set("estado", estadoLote);
        }

        if (orm.update()) {
            msg_exito("Lote actualizado correctamente.");
        } else {
            msg_error("Ocurrio un problema al actualizar. Revise el ID.");
        }
    }

    void eliminar_lote(EloquentORM& orm) {
        limpiar();
        cout << ROJO << "      ELIMINAR LOTE" << RESET << endl;
        mostrar_todos(orm);
        string idStr = to_string(leer_entero("\nID del lote a eliminar: "));
        
        orm.set("id", idStr); 

        if (orm.remove()) {
            msg_exito("Lote eliminado del sistema.");
        } else {
            msg_error("No se pudo eliminar.");
        }
    }
};

// ============================================================
//  BLOQUE 5: MENUS DE CADA MODULO
// ============================================================
void menu_proyectos(EloquentORM& modProyecto) {
    int opcion;
    do {
        limpiar();
        cout << AMARILLO; linea();
        cout << "   MODULO 1 - GESTION DE PROYECTOS" << endl;
        linea(); cout << RESET;
        cout << "  1. Registrar nuevo proyecto" << endl;
        cout << "  2. Ver todos los proyectos"  << endl;
        cout << "  3. Editar un proyecto"        << endl;
        cout << "  4. Eliminar un proyecto"      << endl;
        cout << "  0. Volver al menu principal"  << endl;
        linea();

        opcion = leer_entero("  Seleccione una opcion: ");
        Proyecto p;
        switch (opcion) {
            case 1: p.registrar_nuevo_proyecto(modProyecto); pausa(); break;
            case 2: limpiar(); Proyecto::mostrar_todos(modProyecto); pausa(); break;
            case 3: p.actualizar_proyecto(modProyecto); pausa(); break;
            case 4: p.eliminar_proyecto(modProyecto); pausa(); break;
            case 0: msg_aviso("Volviendo al menu principal..."); break;
            default: msg_error("Opcion no valida."); pausa(); break;
        }
    } while (opcion != 0);
}

void menu_lotes(EloquentORM& modLote, EloquentORM& modProyecto) {
    int opcion;
    do {
        limpiar();
        cout << AMARILLO; linea();
        cout << "    MODULO 2 - GESTION DE LOTES" << endl;
        linea(); cout << RESET;
        cout << "  1. Registrar nuevo lote" << endl;
        cout << "  2. Ver todos los lotes"  << endl;
        cout << "  3. Editar un lote"       << endl;
        cout << "  4. Eliminar un lote"     << endl;
        cout << "  0. Volver al menu principal" << endl;
        linea();

        opcion = leer_entero("  Seleccione una opcion: ");
        Lote l;
        switch (opcion) {
            case 1: l.registrar_nuevo_lote(modLote, modProyecto); pausa(); break;
            case 2: limpiar(); Lote::mostrar_todos(modLote); pausa(); break;
            case 3: l.actualizar_lote(modLote); pausa(); break;
            case 4: l.eliminar_lote(modLote); pausa(); break;
            case 0: msg_aviso("Volviendo al menu principal..."); break;
            default: msg_error("Opcion no valida."); pausa(); break;
        }
    } while (opcion != 0);
}

// ============================================================
//  BLOQUE 6: MAIN
// ============================================================
int main() {
    MySQLConexion db("root", "1234", "lotificadora_db");

    if (!db.open()) {
        msg_error("No se pudo conectar a MySQL.");
        return 1;
    }

    cout << VERDE << "[OK] Conexion exitosa a 'lotificadora_db'." << RESET << endl;
    pausa();

    EloquentORM modProyecto(db, "proyectos", {"nombre_proyecto", "ubicacion"});
    EloquentORM modLote(db, "lotes", {"numero_lote", "precio", "estado", "proyecto_id"});

    int opcion;
    do {
        limpiar();
        cout << CYAN; linea();
        cout << "    SISTEMA DE LOTIFICADORA (UMG)" << endl;
        linea(); cout << RESET;
        cout << "  1. Modulo - Proyectos" << endl;
        cout << "  2. Modulo - Lotes"     << endl;
        cout << "  0. Salir del sistema"  << endl;
        linea();

        opcion = leer_entero("  Seleccione: ");
        switch (opcion) {
            case 1: menu_proyectos(modProyecto); break;
            case 2: menu_lotes(modLote, modProyecto); break;
            case 0: msg_exito("Saliendo..."); break;
            default: msg_error("Opcion no valida."); pausa(); break;
        }
    } while (opcion != 0);

    db.close();
    return 0;
}