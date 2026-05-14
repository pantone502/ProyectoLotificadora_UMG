-- ============================================================
--  SCRIPT SQL - SISTEMA DE LOTIFICADORA
--  Universidad Mariano Galvez de Guatemala
--  Curso    : Programacion en C++
--  Autores  : [Nombre 1], [Nombre 2], [Nombre 3]
--  Fecha    : 13/05/2026
--
--  Descripcion:
--  Este script crea la base de datos completa del sistema.
--  Incluye las tablas: proyectos y lotes con su relacion FK.
--
--  Como usarlo:
--  1. Abrir MySQL Workbench
--  2. Ir a File > Open SQL Script y seleccionar este archivo
--  3. Presionar el rayo (Execute) para correrlo completo
--  O tambien desde CMD:
--     mysql -u root -p < script.sql
-- ============================================================


-- ------------------------------------------------------------
--  PASO 1: Crear la base de datos si no existe
--  Usamos IF NOT EXISTS para que no falle si ya estaba creada
-- ------------------------------------------------------------
CREATE DATABASE IF NOT EXISTS lotificadora_db
    CHARACTER SET utf8
    COLLATE utf8_general_ci;

-- Nos movemos a usar esa base de datos
USE lotificadora_db;


-- ------------------------------------------------------------
--  PASO 2: Tabla PROYECTOS
--
--  Es la tabla "padre" de la relacion.
--  Un proyecto puede tener muchos lotes.
--
--  Columnas:
--    id              -> Clave primaria, se genera sola (AUTO_INCREMENT)
--    nombre_proyecto -> Nombre del proyecto, ej: "Residenciales Tikal"
--    ubicacion       -> Direccion o zona del proyecto
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS proyectos (
    id              INT          NOT NULL AUTO_INCREMENT,
    nombre_proyecto VARCHAR(100) NOT NULL,
    ubicacion       VARCHAR(100) NOT NULL,

    -- Definimos 'id' como la clave primaria de esta tabla
    PRIMARY KEY (id)
);


-- ------------------------------------------------------------
--  PASO 3: Tabla LOTES
--
--  Es la tabla "hija" de la relacion.
--  Cada lote pertenece a UN proyecto mediante proyecto_id (FK).
--
--  Columnas:
--    id           -> Clave primaria, se genera sola
--    numero_lote  -> Identificador del lote, ej: "L-001"
--    precio       -> Precio de venta, con 2 decimales
--    estado       -> Situacion actual: disponible/vendido/reservado
--    proyecto_id  -> Clave foranea que apunta a proyectos.id
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS lotes (
    id          INT            NOT NULL AUTO_INCREMENT,
    numero_lote VARCHAR(20)    NOT NULL,
    precio      DECIMAL(10, 2) NOT NULL,
    estado      VARCHAR(20)    NOT NULL DEFAULT 'disponible',
    proyecto_id INT            NOT NULL,

    -- Clave primaria de esta tabla
    PRIMARY KEY (id),

    -- Clave foranea: proyecto_id apunta al id de la tabla proyectos
    -- ON DELETE CASCADE significa: si se borra el proyecto,
    -- se borran automaticamente todos sus lotes tambien
    CONSTRAINT fk_lote_proyecto
        FOREIGN KEY (proyecto_id)
        REFERENCES proyectos (id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);


-- ============================================================
--  PASO 4: DATOS DE PRUEBA
--
--  Insertamos registros iniciales para poder probar el sistema
--  desde el primer momento sin tener que cargar todo a mano.
-- ============================================================

-- Proyectos de ejemplo
INSERT INTO proyectos (nombre_proyecto, ubicacion) VALUES
    ('Residenciales Tikal',   'Zona 7, Ciudad de Guatemala'),
    ('Lotificadora Peten',    'Santa Elena, Peten'),
    ('Villas del Lago',       'Amatitlan, Guatemala'),
    ('Residenciales del Sur', 'Villa Nueva, Guatemala');

-- Lotes de ejemplo asociados a los proyectos anteriores
-- Los proyecto_id deben coincidir con los id de la tabla proyectos
INSERT INTO lotes (numero_lote, precio, estado, proyecto_id) VALUES
    ('L-001', 85000.00,  'disponible', 1),
    ('L-002', 92000.00,  'vendido',    1),
    ('L-003', 78500.00,  'reservado',  1),
    ('L-004', 120000.00, 'disponible', 2),
    ('L-005', 115000.00, 'disponible', 2),
    ('L-006', 65000.00,  'vendido',    3),
    ('L-007', 70000.00,  'disponible', 4);


-- ============================================================
--  PASO 5: VERIFICACION
--
--  Estas consultas muestran el contenido de las tablas para
--  confirmar que todo se creo e inserto correctamente.
-- ============================================================

-- Ver todos los proyectos
SELECT * FROM proyectos;

-- Ver todos los lotes
SELECT * FROM lotes;

-- Ver lotes con el nombre del proyecto al que pertenecen
-- (esto es un JOIN: combina informacion de dos tablas)
SELECT
    l.id           AS lote_id,
    l.numero_lote,
    l.precio,
    l.estado,
    p.nombre_proyecto,
    p.ubicacion
FROM lotes l
INNER JOIN proyectos p ON l.proyecto_id = p.id
ORDER BY p.id, l.id;

-- ============================================================
--  FIN DEL SCRIPT
-- ============================================================