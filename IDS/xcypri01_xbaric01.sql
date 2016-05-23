/**
 * Subor: xcypri01_xbaric01.sql
 * Autori: Michal Cyprian <xcypri01@stud.fit.vutbr.cz>, 
 *         Filip Baric <xbaric01@stud.fit.vutbr.cz>
 *
 * SQL skript pro vytvoření pokročilých objektů schématu databáze 
 * stvrta cast projektu pre predmet IDS, VUT Brno 2015/2016
 */


/*
 * Odstranenie vsetkych tabuliek
 */
DROP TABLE Zamestnanec CASCADE CONSTRAINTS;
DROP TABLE Udalost CASCADE CONSTRAINTS;
DROP TABLE Skupina CASCADE CONSTRAINTS;
DROP TABLE Tim CASCADE CONSTRAINTS;
DROP TABLE Zapas CASCADE CONSTRAINTS;
DROP TABLE Clen_timu CASCADE CONSTRAINTS;
DROP TABLE Rozhodca CASCADE CONSTRAINTS;
DROP TABLE Formacia CASCADE CONSTRAINTS;
DROP TABLE Patri_do CASCADE CONSTRAINTS;
DROP TABLE Rozhodoval CASCADE CONSTRAINTS;
DROP TABLE Hral_v CASCADE CONSTRAINTS;

DROP SEQUENCE zamestnanec_id_seq;
DROP PROCEDURE vyhry_prehry;
DROP PROCEDURE data_zapasu;


/*
 * Vytvorenie tabuliek
 */
CREATE TABLE Zamestnanec (
    id_zamestnanca INTEGER,
    meno VARCHAR(255) NOT NULL,
    priezvisko VARCHAR(255) NOT NULL,
    datum_narodenia DATE NOT NULL,
    pracovna_pozicia VARCHAR(255)
);

CREATE TABLE Udalost (
    cislo_udalosti INTEGER,
    typ VARCHAR(255) NOT NULL,
    cas DATE NOT NULL,
    informacie VARCHAR(255),
    id_zamestnanca INTEGER NOT NULL,
    id_zapasu INTEGER NOT NULL,
    id_clena INTEGER NOT NULL
);

CREATE TABLE Skupina (
    kod_skupiny VARCHAR(1)
);

CREATE TABLE Tim (  
     nazov_timu VARCHAR(255),
     skratka VARCHAR(3),
     pocet_clenov INT,
     pocet_odohratych_zapasov INTEGER NOT NULL,
     vyhry INTEGER NOT NULL,
     vyhry_po_predlzeni INTEGER NOT NULL,
     prehry INTEGER NOT NULL,
     prehry_po_predlzeni INTEGER NOT NULL,
     skupina VARCHAR(1) NOT NULL,
     max_clenov INT NOT NULL 
);

CREATE TABLE Zapas (
    id_zapasu INTEGER,
    typ VARCHAR(255) NOT NULL,
    datum_konania DATE NOT NULL,
    miesto_konania VARCHAR(255),
    skore_domacich INTEGER,
    skore_hosti INTEGER,
    pocet_divakov INTEGER,
    pocet_predlzeni INTEGER,
    id_skupiny VARCHAR(1) NOT NULL,
    tim_domacich VARCHAR(255),
    tim_hosti VARCHAR(255)
);

CREATE TABLE Clen_timu (
    id_clena INTEGER,
    meno VARCHAR(255) NOT NULL,
    priezvisko VARCHAR(255) NOT NULL,
    datum_narodenia DATE NOT NULL,
    rola VARCHAR(255) NOT NULL,
    cislo_dresu INTEGER CHECK (cislo_dresu > 0 AND cislo_dresu < 100),
    herna_pozicia VARCHAR(255),
    domovsky_klub VARCHAR(255),
    nazov_timu VARCHAR(255)
);

CREATE TABLE Rozhodca (
    id_rozhodcu INTEGER,
    meno VARCHAR(255) NOT NULL,
    priezvisko VARCHAR(255) NOT NULL,
    datum_narodenia DATE NOT NULL
);

CREATE TABLE Formacia (
    cislo_formacie INTEGER,
    rola_timu VARCHAR(255),
    id_zapasu INTEGER
);

CREATE TABLE Patri_do (
    ziskane_body INTEGER,
    nazov_timu VARCHAR(255),
    kod_skupiny VARCHAR(1)
);

CREATE TABLE Rozhodoval (
    rola VARCHAR(255),
    id_zapasu INTEGER,
    id_rozhodcu INTEGER
);

CREATE TABLE Hral_v (
    straveny_cas DATE NOT NULL,
    id_clena INTEGER,
    cislo_formacie INTEGER,
    cislo_zapasu INTEGER
);

/*
 * Nastavene primarnych a cudzych klucov
 */
ALTER TABLE Zamestnanec ADD CONSTRAINT PK_zamestnanec PRIMARY KEY (id_zamestnanca);
ALTER TABLE Skupina ADD CONSTRAINT PK_skupina PRIMARY KEY (kod_skupiny);
ALTER TABLE Zapas ADD CONSTRAINT PK_zapas PRIMARY KEY (id_zapasu);
ALTER TABLE Tim ADD CONSTRAINT PK_tim PRIMARY KEY (nazov_timu);
ALTER TABLE Clen_timu ADD CONSTRAINT PK_clen PRIMARY KEY (id_clena);
ALTER TABLE Udalost ADD CONSTRAINT PK_udalost PRIMARY KEY (cislo_udalosti, id_zapasu);
ALTER TABLE Rozhodca ADD CONSTRAINT PK_rozhodca PRIMARY KEY (id_rozhodcu);
ALTER TABLE Formacia ADD CONSTRAINT PK_formacia PRIMARY KEY (cislo_formacie, id_zapasu);
ALTER TABLE Patri_do ADD CONSTRAINT PK_patri_do PRIMARY KEY (ziskane_body, nazov_timu);
ALTER TABLE Rozhodoval ADD CONSTRAINT PK_rozhodoval PRIMARY KEY (rola, id_zapasu);
ALTER TABLE Hral_v ADD CONSTRAINT PK_hral_v PRIMARY KEY (straveny_cas, id_clena);

ALTER TABLE Zapas ADD CONSTRAINT FK_skupina FOREIGN KEY (id_skupiny) REFERENCES Skupina;
ALTER TABLE Zapas ADD CONSTRAINT FK_tim_domacich FOREIGN KEY (tim_domacich) REFERENCES Tim;
ALTER TABLE Zapas ADD CONSTRAINT FK_tim_hosti FOREIGN KEY (tim_hosti) REFERENCES Tim;
ALTER TABLE Tim ADD CONSTRAINT FK_id_skupiny FOREIGN KEY (skupina) REFERENCES Skupina;
ALTER TABLE Clen_timu ADD CONSTRAINT FK_tim FOREIGN KEY (nazov_timu) REFERENCES Tim;
ALTER TABLE Udalost ADD CONSTRAINT FK_zodpovedny_zamestnanec FOREIGN KEY (id_zamestnanca) REFERENCES Zamestnanec;
ALTER TABLE Udalost ADD CONSTRAINT FK_zapas FOREIGN KEY (id_zapasu) REFERENCES Zapas ON DELETE CASCADE;
ALTER TABLE Udalost ADD CONSTRAINT FK_akter FOREIGN KEY (id_clena) REFERENCES Clen_timu;
ALTER TABLE Formacia ADD CONSTRAINT FK_id_zapasu FOREIGN KEY (id_zapasu) REFERENCES Zapas ON DELETE CASCADE;
ALTER TABLE Patri_do ADD CONSTRAINT FK_tim_skupiny FOREIGN KEY (nazov_timu) REFERENCES Tim;
ALTER TABLE Patri_do ADD CONSTRAINT FK_skupina_timu FOREIGN KEY (kod_skupiny) REFERENCES Skupina;
ALTER TABLE Rozhodoval ADD CONSTRAINT FK_rozhodca FOREIGN KEY (id_rozhodcu) REFERENCES Rozhodca;
ALTER TABLE Rozhodoval ADD CONSTRAINT FK_rozhodoval_zapas FOREIGN KEY (id_zapasu) REFERENCES Zapas ON DELETE CASCADE;
ALTER TABLE Hral_v ADD CONSTRAINT FK_clen FOREIGN KEY (id_clena) REFERENCES Clen_timu;
ALTER TABLE Hral_v ADD CONSTRAINT FK_formacia FOREIGN KEY (cislo_formacie,cislo_zapasu) REFERENCES Formacia;

/*
 * Vkladanie ukazokvych hodnot
 */
 
CREATE OR REPLACE TRIGGER clen_tim_add
BEFORE INSERT
ON Clen_timu
FOR EACH ROW
DECLARE
	pocet INT;
	max_c INT;
BEGIN
SELECT pocet_clenov INTO pocet FROM Tim WHERE nazov_timu=:new.nazov_timu;
SELECT max_clenov INTO max_c FROM Tim WHERE nazov_timu=:new.nazov_timu;
IF pocet + 1 > max_c THEN
	raise_application_error(-20002, 'Prekroceny maximalny pocet clenov');
ELSE
	UPDATE Tim SET pocet_clenov=Tim.pocet_clenov + 1 WHERE nazov_timu=:new.nazov_timu;
END IF;
END;
/
 
CREATE SEQUENCE zamestnanec_id_seq
	increment by 1
	start with 1
	nomaxvalue
	nocache
	nocycle; 
 
 
CREATE OR REPLACE TRIGGER id_gen
BEFORE INSERT
ON Zamestnanec
FOR EACH ROW
BEGIN
  IF: new.id_zamestnanca IS NULL THEN
    SELECT zamestnanec_id_seq.NEXTVAL INTO: new.id_zamestnanca FROM DUAL;
  END IF;
END;
/

CREATE PROCEDURE vyhry_prehry(kod_skupiny VARCHAR) AS
    c_nazov Tim.nazov_timu%TYPE;
    c_vyhry Tim.vyhry%TYPE;
    c_vyhry_po_pred Tim.vyhry_po_predlzeni%TYPE;
    c_prehry Tim.prehry%TYPE;
    c_prehry_po_pred Tim.prehry_po_predlzeni%TYPE;
    CURSOR c_sumar_zapasov IS
        SELECT nazov_timu, vyhry, vyhry_po_predlzeni, prehry, prehry_po_predlzeni FROM Tim;     
  BEGIN
   OPEN c_sumar_zapasov;
   LOOP
       FETCH c_sumar_zapasov into c_nazov, c_vyhry, c_vyhry_po_pred, c_prehry, c_prehry_po_pred;
       EXIT WHEN c_sumar_zapasov%notfound;
       DBMS_OUTPUT.put_line(c_nazov || 'vyhry spolu: ' ||TO_CHAR(c_vyhry + c_vyhry_po_pred) || ', prehry spolu: '|| TO_CHAR(c_prehry + c_prehry_po_pred));
   END LOOP;
   CLOSE c_sumar_zapasov;
END;
/
 
CREATE PROCEDURE data_zapasu(moj_zapas INT) AS
    c_datum Zapas.datum_konania%TYPE;
    c_miesto Zapas.miesto_konania%TYPE;
BEGIN
    SELECT datum_konania, miesto_konania INTO c_datum, c_miesto
    FROM Zapas WHERE id_zapasu = moj_zapas;
    
    DBMS_OUTPUT.put_line('Zapas ' || moj_zapas || ' datum: '|| c_datum || ' miesto: ' || c_miesto);
EXCEPTION
    WHEN no_data_found THEN
        DBMS_OUTPUT.put_line('Zapas neexistuje.');
    WHEN others THEN
        DBMS_OUTPUT.put_line('Error.');
END;
/
    
INSERT INTO Skupina (kod_skupiny) VALUES ('A'); 
INSERT INTO Skupina (kod_skupiny) VALUES ('B');
 
INSERT INTO Tim (nazov_timu, skratka, pocet_clenov, pocet_odohratych_zapasov, vyhry, vyhry_po_predlzeni, prehry, prehry_po_predlzeni,skupina,max_clenov)
VALUES ('Ceska Republika','CZE', '0', '2', '0', '1', '1', '0', 'A','40');
INSERT INTO Tim VALUES ('Slovensko','SVK','0', '2', '1', '1', '0', '0', 'A','40');
INSERT INTO Tim VALUES ('USA','USA', '0', '2', '0', '0', '2', '0', 'B','40');
INSERT INTO Tim VALUES ('Nemecko','GER', '0', '1', '1', '0', '1', '0', 'B','40');
INSERT INTO Tim VALUES ('Rusko', 'RUS', '0', '2', '2', '0', '0', '0', 'B','40');
INSERT INTO Tim VALUES ('Kanada', 'CAN', '0', '2', '0', '1', '1', '0', 'A','40');
 
INSERT INTO Zamestnanec (id_zamestnanca, meno, priezvisko, datum_narodenia, pracovna_pozicia)
VALUES ('', 'Martin', 'Novak', TO_DATE('1993-11-10', 'YYYY-MM-DD'), 'upratovac');
INSERT INTO Zamestnanec (id_zamestnanca, meno, priezvisko, datum_narodenia, pracovna_pozicia)
VALUES ('', 'Peter', 'Novak', TO_DATE('1994-10-10', 'YYYY-MM-DD'), 'spravca');
 
INSERT INTO Zapas (id_zapasu, typ, datum_konania, miesto_konania, skore_domacich, skore_hosti, pocet_divakov, pocet_predlzeni, id_skupiny, tim_domacich, tim_hosti) 
VALUES ('1','skupinovy',TO_DATE('2016-05-09', 'YYYY-MM-DD'),'Bratislava','1','0','10000','1','A','Slovensko','Ceska Republika');
INSERT INTO Zapas (id_zapasu, typ, datum_konania, miesto_konania, skore_domacich, skore_hosti, pocet_divakov, pocet_predlzeni, id_skupiny, tim_domacich, tim_hosti) 
VALUES ('2','skupinovy',TO_DATE('2016-05-07', 'YYYY-MM-DD'),'Kosice','3','5','12500','0','B','USA','Nemecko');

INSERT INTO Clen_timu (id_clena, meno, priezvisko, datum_narodenia, rola, cislo_dresu, herna_pozicia, domovsky_klub, nazov_timu)
VALUES ('1','Zdeno','Chara', TO_DATE('1977-03-18', 'YYYY-MM-DD'),'hrac','33','obranca','Boston Bruins','Slovensko');
INSERT INTO Clen_timu (id_clena, meno, priezvisko, datum_narodenia, rola, cislo_dresu, herna_pozicia, domovsky_klub, nazov_timu)
VALUES ('2','Zdeno','Ciger', TO_DATE('1969-10-19', 'YYYY-MM-DD'),'trener','','','','Slovensko');
INSERT INTO Clen_timu VALUES ('3','Sidney','Crosby', TO_DATE('1987-3-5', 'YYYY-MM-DD'),'hrac','87','utocnik','Pittsburgh Penguins','Kanada');
INSERT INTO Clen_timu VALUES ('4','Yevgeni','Malkin', TO_DATE('1986-7-31', 'YYYY-MM-DD'),'hrac','11','utocnik','Pittsburgh Penguins','Rusko');
INSERT INTO Clen_timu VALUES ('5','Alex','Ovechkin', TO_DATE('1985-9-17', 'YYYY-MM-DD'),'hrac','8','utocnik','Washington Capitals','Rusko');
INSERT INTO Clen_timu VALUES ('6','Milan','Bartovic', TO_DATE('1981-4-9', 'YYYY-MM-DD'),'hrac','12','utocnik','Slovan Bratislava','Slovensko');
INSERT INTO Clen_timu VALUES ('7','Jaromir','Jagr', TO_DATE('1972-2-15', 'YYYY-MM-DD'),'hrac','68','utocnik','Florida Panthers','Ceska Republika');
INSERT INTO Clen_timu VALUES ('8','Andrej','Meszaros', TO_DATE('1985-9-13', 'YYYY-MM-DD'),'hrac','14','obranca','Buffalo Sabres','Slovensko');
INSERT INTO Clen_timu VALUES ('9','Marian','Gaborik', TO_DATE('1983-2-22', 'YYYY-MM-DD'),'hrac','10','utocnik','LA Kings','Slovensko');

INSERT INTO Udalost (cislo_udalosti, typ, cas, informacie, id_zamestnanca, id_zapasu, id_clena)
VALUES ('1', 'gol', TO_DATE('00:19:21', 'HH24:MI:SS'), '', '2', '1', '1');
INSERT INTO Udalost (cislo_udalosti, typ, cas, informacie, id_zamestnanca, id_zapasu, id_clena)
VALUES ('2', 'vylucenie', TO_DATE('00:10:45', 'HH24:MI:SS'), 'hakovanie', '2', '1', '1');
INSERT INTO Udalost VALUES ('4', 'gol', TO_DATE('00:34:01', 'HH24:MI:SS'), '', '2', '1', '7');
INSERT INTO Udalost VALUES ('5', 'gol', TO_DATE('00:39:01', 'HH24:MI:SS'), '', '2', '1', '9');
INSERT INTO Udalost VALUES ('6', 'gol', TO_DATE('00:40:05', 'HH24:MI:SS'), '', '2', '1', '7');
INSERT INTO Udalost VALUES ('10', 'vylucenie', TO_DATE('00:44:59', 'HH24:MI:SS'), '', '2', '1', '7');
INSERT INTO Udalost VALUES ('7', 'gol', TO_DATE('00:45:25', 'HH24:MI:SS'), '', '2', '1', '8');
INSERT INTO Udalost VALUES ('8', 'gol', TO_DATE('00:59:59', 'HH24:MI:SS'), '', '2', '1', '7');
INSERT INTO Udalost VALUES ('9', 'gol', TO_DATE('01:03:59', 'HH24:MI:SS'), '', '2', '1', '6');


INSERT INTO Rozhodca (id_rozhodcu, meno, priezvisko, datum_narodenia)
VALUES ('1','Vladimir','Baluska',TO_DATE('1975-01-30', 'YYYY-MM-DD'));
INSERT INTO Rozhodca (id_rozhodcu, meno, priezvisko, datum_narodenia)
VALUES ('2','Tomas','Orolin',TO_DATE('1980-06-22', 'YYYY-MM-DD'));

INSERT INTO Formacia (cislo_formacie, rola_timu, id_zapasu) VALUES ('1','','1');
INSERT INTO Formacia (cislo_formacie, rola_timu, id_zapasu) VALUES ('2','','2');

INSERT INTO Patri_do (ziskane_body, nazov_timu, kod_skupiny) VALUES ('2','Slovensko','A');
INSERT INTO Patri_do (ziskane_body, nazov_timu, kod_skupiny) VALUES ('1','Ceska Republika','A');
INSERT INTO Patri_do (ziskane_body, nazov_timu, kod_skupiny) VALUES ('0','USA','B');
INSERT INTO Patri_do (ziskane_body, nazov_timu, kod_skupiny) VALUES ('3','Nemecko','B');

INSERT INTO Rozhodoval (rola, id_zapasu, id_rozhodcu) VALUES ('hlavny','1','1');
INSERT INTO Rozhodoval (rola, id_zapasu, id_rozhodcu) VALUES ('hlavny','2','1');
INSERT INTO Rozhodoval (rola, id_zapasu, id_rozhodcu) VALUES ('ciarovy','1','2');
INSERT INTO Rozhodoval (rola, id_zapasu, id_rozhodcu) VALUES ('ciarovy','2','2');

INSERT INTO Hral_v (straveny_cas, id_clena, cislo_formacie, cislo_zapasu) VALUES (TO_DATE('00:28:43', 'HH24:MI:SS'),'1','1','1');

-- Explain plan bez nastavenia indexu
EXPLAIN PLAN SET STATEMENT_ID = 'moj_plan' FOR
SELECT skratka, COUNT(*) pocet_golov
FROM Udalost NATURAL JOIN Clen_timu NATURAL JOIN Tim
WHERE typ='gol'
GROUP BY skratka;

SELECT plan_table_output FROM table(DBMS_XPLAN.display('plan_table', 'moj_plan', 'typical'));
-- Tvorba indexu na typ z tabulky Udalost
CREATE INDEX ind ON Udalost(typ);

-- Demonstracia optimalizacie pristupu s indexom
EXPLAIN PLAN SET STATEMENT_ID = 'moj_plan' FOR
SELECT skratka, COUNT(*) pocet_golov
FROM Udalost NATURAL JOIN Clen_timu NATURAL JOIN Tim
WHERE typ='gol'
GROUP BY skratka;

SELECT plan_table_output FROM table(DBMS_XPLAN.display('plan_table', 'moj_plan', 'typical'));

-- Demonstracia funkcnosti prveho triggeru (obmedzenie poctu clenov timu), 3. INSERT vyvola chybu prekrocenia maximalneho poctu clenov
INSERT INTO Tim VALUES ('Ukrajina', 'UKR', '0', '0', '0', '0', '0', '0', 'B','2');
INSERT INTO Clen_timu VALUES ('20','Dmytro','Chrystyc', TO_DATE('1983-2-22', 'YYYY-MM-DD'),'hrac','10','utocnik','Washington Capitals','Ukrajina');
INSERT INTO Clen_timu VALUES ('21','Vjaceslav','Buracikov', TO_DATE('1983-2-22', 'YYYY-MM-DD'),'hrac','10','utocnik','CSK Moskva','Ukrajina');
--INSERT INTO Clen_timu VALUES ('21','Vasilij','Koseckin', TO_DATE('1983-2-22', 'YYYY-MM-DD'),'hrac','10','obranca','Metallurg Magnitogorsk','Ukrajina');

-- Demostracia druheho triggeru (automaticke generovanie primarnych klucov)
INSERT INTO Zamestnanec (id_zamestnanca, meno, priezvisko, datum_narodenia, pracovna_pozicia)
VALUES ('', 'Martin', 'Benuska', TO_DATE('1993-11-10', 'YYYY-MM-DD'), 'upratovac');

SELECT id_zamestnanca FROM Zamestnanec ;

-- Zavolanie definovanych procedur
CALL vyhry_prehry('A');
CALL data_zapasu('1');

GRANT ALL ON Zamestnanec TO xbaric01;
GRANT ALL ON Udalost TO xbaric01;
GRANT ALL ON Skupina TO xbaric01;
GRANT ALL ON Tim TO xbaric01;
GRANT ALL ON Zapas TO xbaric01;
GRANT ALL ON Clen_timu TO xbaric01;
GRANT ALL ON Rozhodca TO xbaric01;
GRANT ALL ON Formacia TO xbaric01;
GRANT ALL ON Patri_do TO xbaric01;
GRANT ALL ON Rozhodoval TO xbaric01;
GRANT ALL ON vyhry_prehry TO xbaric01;
GRANT ALL ON data_zapasu TO xbaric01;
--GRANT CREATE VIEW TO xbaric01;


COMMIT;

CREATE MATERIALIZED VIEW LOG ON xcypri01.Clen_timu WITH ROWID;
CREATE MATERIALIZED VIEW LOG ON xcypri01.Tim WITH ROWID;


CONNECT xbaric01/********/;
DROP MATERIALIZED VIEW xbaric01.realizacny_tim;

 -- Materializovany pohlad kompletnych dat vsetkych clenov timov bez prehry, ktory nie su hracmi.
CREATE MATERIALIZED VIEW xbaric01.realizacny_tim
NOLOGGING
CACHE
BUILD IMMEDIATE
REFRESH FAST ON COMMIT
ENABLE QUERY REWRITE
AS 
   
SELECT id_clena, meno , priezvisko, xcypri01.Clen_timu.ROWID AS clen_timu_rowid,
xcypri01.Tim.ROWID AS tim_rowid FROM xcypri01.Clen_timu NATURAL JOIN  xcypri01.Tim
WHERE rola<>'hrac' AND prehry='0';

SELECT * FROM xbaric01.realizacny_tim;
