INSERT INTO Transacciones (numero_cuenta, tipo_movimiento, monto, cuenta_destino) VALUES (501, 'Deposito', 600, NULL);

-- ACTULIZACIÓN DE SALDOS GENERALES
INSERT INTO Cuentas (numero_cuenta, nombre_titular, saldo_actual, tipo_cuenta, correo_electronico) VALUES (101, 'Carlos Lopez', 1500, 'Monetaria', 'carlos@gmail.com') ON DUPLICATE KEY UPDATE saldo_actual = 1500;
INSERT INTO Cuentas (numero_cuenta, nombre_titular, saldo_actual, tipo_cuenta, correo_electronico) VALUES (202, 'Maria Gomez', 500, 'Ahorro', 'maria@gmail.com') ON DUPLICATE KEY UPDATE saldo_actual = 500;
INSERT INTO Cuentas (numero_cuenta, nombre_titular, saldo_actual, tipo_cuenta, correo_electronico) VALUES (501, 'Dan', 1400, 'M', 'wm@gmail.com') ON DUPLICATE KEY UPDATE saldo_actual = 1400;
