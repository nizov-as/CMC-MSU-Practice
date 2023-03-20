-- 1. ЗАПРОСЫ SQL К СВЯЗАННЫМ ТАБЛИЦАМ
/*  */


--------------------------------------------
-- 2.2 Запросы на выборку, соединение таблиц
--------------------------------------------

/* Оператор внутреннего соединения INNER JOIN соединяет две таблицы. Результат запроса формируется так:
• каждая строка одной таблицы сопоставляется с каждой строкой второй таблицы;
• для полученной «соединённой» строки проверяется условие соединения;
• если условие истинно, в таблицу результата добавляется соответствующая «соединённая» строка; */

/* Вывести название книг и их авторов. */
SELECT title, name_author
FROM 
    author INNER JOIN book
    ON author.author_id = book.author_id;

/* Вывести название, жанр и цену тех книг, количество которых больше 8, в отсортированном по убыванию цены виде. */
SELECT
    title
    ,name_genre
    ,price
FROM 
    genre INNER JOIN book 
    ON genre.genre_id = book.genre_id
WHERE book.amount > 8
ORDER BY price DESC;

/* LEFT JOIN. Результат запроса формируется так:
1. в результат включается внутреннее соединение (INNER JOIN) первой и второй таблицы в соответствии с условием;
2. затем в результат добавляются те записи первой таблицы, которые не вошли во внутреннее соединение на шаге 1, для таких записей соответствующие поля второй таблицы заполняются значениями NULL. */

/* Вывести название всех книг каждого автора, если книг некоторых авторов в данный момент нет на складе – вместо названия книги указать Null. */
SELECT name_author, title 
FROM author LEFT JOIN book
    ON author.author_id = book.author_id
ORDER BY name_author;

/* Вывести все жанры, которые не представлены в книгах на складе. */
SELECT
    name_genre
FROM 
    genre LEFT JOIN book
    ON genre.genre_id = book.genre_id
WHERE amount IS NULL;

/* Необходимо в каждом городе провести выставку книг каждого автора в течение 2020 года. Дату проведения выставки выбрать случайным образом. Создать запрос, который выведет город, автора и дату проведения выставки. Последний столбец назвать Дата. Информацию вывести, отсортировав сначала в алфавитном порядке по названиям городов, а потом по убыванию дат проведения выставок. */
SELECT
    name_city
    ,name_author
    ,DATE_ADD('2020-01-01', INTERVAL FLOOR(RAND()*365) DAY) as Дата
FROM 
    city CROSS JOIN author
ORDER BY name_city ASC, Дата DESC;

/* Вывести информацию о тех книгах, их авторах и жанрах, цена которых принадлежит интервалу от 500  до 700 рублей  включительно. */
SELECT title, name_author, name_genre, price, amount
FROM
    author 
    INNER JOIN book ON author.author_id = book.author_id
    INNER JOIN genre ON genre.genre_id = book.genre_id
WHERE price BETWEEN 500 AND 700;

/* Вывести информацию о книгах (жанр, книга, автор), относящихся к жанру, включающему слово «роман» в отсортированном по названиям книг виде. */
SELECT
    name_genre
    ,title
    ,name_author
FROM
    genre
    INNER JOIN book ON genre.genre_id = book.genre_id
    INNER JOIN author ON book.author_id = author.author_id
WHERE name_genre = "Роман"
ORDER BY title ASC;

/* Вывести количество различных книг каждого автора. Информацию отсортировать в алфавитном порядке по фамилиям авторов. */
SELECT name_author, count(title) AS Количество
FROM
    author INNER JOIN book
    ON author.author_id = book.author_id
GROUP BY name_author
ORDER BY name_author;

/* Посчитать количество экземпляров  книг каждого автора из таблицы author.  Вывести тех авторов,  количество книг которых меньше 10, в отсортированном по возрастанию количества виде. Последний столбец назвать Количество. */
SELECT
    name_author
    ,SUM(amount) AS Количество
FROM
    author LEFT JOIN book
    ON author.author_id = book.author_id
GROUP BY name_author
HAVING Количество < 10 OR SUM(amount) IS NULL
ORDER BY Количество;

/* Вывести авторов, общее количество книг которых на складе максимально. */
SELECT name_author, SUM(amount) as Количество
FROM 
    author INNER JOIN book
    on author.author_id = book.author_id
GROUP BY name_author
HAVING SUM(amount) = 
    ( /* вычисляем максимальное из общего количества книг каждого автора */
    SELECT MAX(sum_amount) AS max_sum_amount
    FROM 
        ( /* считаем количество книг каждого автора */
        SELECT author_id, SUM(amount) AS sum_amount 
        FROM book GROUP BY author_id
        ) query_in
    );

/* Вывести в алфавитном порядке всех авторов, которые пишут только в одном жанре. Поскольку у нас в таблицах так занесены данные, что у каждого автора книги только в одном жанре,  для этого запроса внесем изменения в таблицу book. Пусть у нас  книга Есенина «Черный человек» относится к жанру «Роман», а книга Булгакова «Белая гвардия» к «Приключениям» (эти изменения в таблицы уже внесены). */
SELECT
    author.name_author
FROM 
    genre 
    INNER JOIN book ON book.genre_id = genre.genre_id
    INNER JOIN author ON book.author_id = author.author_id
GROUP BY author.name_author
HAVING COUNT(DISTINCT name_genre) = 1
ORDER BY name_author;

/* Вывести авторов, пишущих книги в самом популярном жанре. Указать этот жанр. */
SELECT  name_author, name_genre
FROM 
    author 
    INNER JOIN book ON author.author_id = book.author_id
    INNER JOIN genre ON  book.genre_id = genre.genre_id
GROUP BY name_author, name_genre, genre.genre_id
HAVING genre.genre_id IN
        (/* выбираем автора, если он пишет книги в самых популярных жанрах*/
        SELECT query_in_1.genre_id
        FROM 
            ( /* выбираем код жанра и количество произведений, относящихся к нему */
            SELECT genre_id, SUM(amount) AS sum_amount
            FROM book
            GROUP BY genre_id
            ) query_in_1
        INNER JOIN 
            ( /* выбираем запись, в которой указан код жанр с максимальным количеством книг */
            SELECT genre_id, SUM(amount) AS sum_amount
            FROM book
            GROUP BY genre_id
            ORDER BY sum_amount DESC
            LIMIT 1
            ) query_in_2
        ON query_in_1.sum_amount = query_in_2.sum_amount
        ); 

/* Вывести информацию о книгах (название книги, фамилию и инициалы автора, название жанра, цену и количество экземпляров книги), написанных в самых популярных жанрах, в отсортированном в алфавитном порядке по названию книг виде. Самым популярным считать жанр, общее количество экземпляров книг которого на складе максимально. */
SELECT title, name_author, name_genre, price, amount
FROM
    genre
    INNER JOIN book ON genre.genre_id = book.genre_id
    INNER JOIN author ON book.author_id = author.author_id
GROUP BY title, name_author, name_genre, price, amount, genre.genre_id
HAVING genre.genre_id IN (
    -- ищем id самых популярных жанров
    SELECT query_in_1.genre_id
    FROM (
        SELECT genre_id, SUM(amount) AS sum_amount
        FROM book
        GROUP BY genre_id
    ) query_in_1
    INNER JOIN (
        SELECT genre_id, SUM(amount) AS sum_amount
        FROM book
        GROUP BY genre_id
        ORDER BY sum_amount DESC
        LIMIT 1
    ) query_in_2
    ON query_in_1.sum_amount = query_in_2.sum_amount
)
ORDER BY title;

/*
Порядок выполнения  SQL запроса на выборку на СЕРВЕРЕ:
1. FROM
2. WHERE
3. GROUP BY
4. HAVING
5. SELECT
6. ORDER BY

Сначала определяется таблица, из которой выбираются данные (FROM), затем из этой таблицы отбираются записи в соответствии с условием  WHERE, выбранные данные агрегируются (GROUP BY), из агрегированных записей выбираются те, которые удовлетворяют условию после HAVING. Потом формируются данные результирующей выборки, как это указано после SELECT ( вычисляются выражения, присваиваются имена и прочее). Результирующая выборка сортируется, как указано после ORDER BY.
*/

/* Если в таблицах supply и book есть одинаковые книги, которые имеют равную цену,  вывести их название и автора, а также посчитать общее количество экземпляров книг в таблицах supply и book,  столбцы назвать Название, Автор  и Количество. */
SELECT
    book.title AS Название
    ,supply.author AS Автор
    ,supply.amount + book.amount AS Количество
FROM 
    supply INNER JOIN book
    ON supply.title = book.title AND supply.price = book.price;


--------------------------------------------------
-- 2.2 Запросы на корректировку, соединение таблиц
--------------------------------------------------

/* Для книг, которые уже есть на складе (в таблице book) по той же цене, что и в поставке (supply), увеличить количество на значение, указанное в поставке, а также обнулить количество этих книг в поставке. */
-- Пояснение: чтобы выполнить сравнение по фамилии автора нужно "подтянуть" таблицу author, которая связана с book по столбцу author_id.
UPDATE book 
     INNER JOIN author ON author.author_id = book.author_id
     INNER JOIN supply ON book.title = supply.title 
                         and supply.author = author.name_author
SET book.amount = book.amount + supply.amount,
    supply.amount = 0   
WHERE book.price = supply.price;

/* Для книг, которые уже есть на складе (в таблице book), но по другой цене, чем в поставке (supply),  необходимо в таблице book увеличить количество на значение, указанное в поставке,  и пересчитать цену. А в таблице  supply обнулить количество этих книг. */
UPDATE
    book
    INNER JOIN author ON book.author_id = author.author_id
    INNER JOIN supply ON book.title = supply.title AND supply.author = author.name_author
SET
    book.price = (book.price*book.amount + supply.price*supply.amount)/(book.amount+supply.amount)
    ,book.amount = book.amount + supply.amount
    ,supply.amount = 0
WHERE book.price <> supply.price;

/* Включить новых авторов в таблицу author с помощью запроса на добавление, а затем вывести все данные из таблицы author.  Новыми считаются авторы, которые есть в таблице supply, но нет в таблице author. */
INSERT INTO author(name_author)
    SELECT supply.author
    FROM supply LEFT JOIN author ON supply.author = author.name_author
    WHERE author.name_author IS NULL;

/* Добавить новые книги из таблицы supply в таблицу book на основе сформированного выше запроса. Затем вывести для просмотра таблицу book. */
INSERT INTO book(title, author_id, price, amount)
SELECT
    title, author_id, price, amount
FROM
    author INNER JOIN supply ON author.name_author = supply.author
WHERE supply.amount <> 0;

/* Занести для книги «Стихотворения и поэмы» Лермонтова жанр «Поэзия», а для книги «Остров сокровищ» Стивенсона - «Приключения». (Использовать два запроса). */
UPDATE book
SET genre_id = (
    SELECT genre_id
    FROM genre
    WHERE name_genre = 'Поэзия'
    )
WHERE book_id = 10;

UPDATE book
SET genre_id = 3
WHERE book_id = 11;

/* Удалить всех авторов, общее количество книг которых меньше 20. */
-- ВАЖНО: нельзя удалять из таблицы данные, основанные на запросе к той же самой таблице (в подзапросе нельзя использовать таблицу author)
DELETE FROM author 
WHERE author_id IN (
    SELECT author_id 
    FROM book
    GROUP BY author_id
    HAVING SUM(amount) < 20
);

/* Удалить все жанры, к которым относится меньше 4-х книг. */
DELETE FROM genre
WHERE genre_id IN (
    SELECT book.genre_id
    FROM book
    GROUP BY genre_id
    HAVING COUNT(genre_id) < 4
    );

/* Удалить всех авторов из таблицы author, у которых есть книги, количество экземпляров которых меньше 3. */
DELETE FROM author
USING 
    author 
    INNER JOIN book ON author.author_id = book.author_id
WHERE book.amount < 3;

/* Удалить всех авторов, которые пишут в жанре "Поэзия". В запросе для отбора авторов использовать полное название жанра, а не его id.*/
DELETE FROM author
USING
    author
    INNER JOIN book ON author.author_id = book.author_id
    INNER JOIN genre ON book.genre_id = genre.genre_id
WHERE name_genre = 'Поэзия';
