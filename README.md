Распаковщик и упаковщик игровых ресурсов PUP
============================================

Предыстория
-----------

В начале 2000-х годах я любил ковыряться в играх и доставать из них различные ресурсы - графику, звуки, модели. Моими главными и единственными инструментами тогда были шестнадцатеричные редакторы pview и hiew, компиляторы Pascal и C/C++. Наковырял я довольно много, правда тогда мне было интересно только доставать из игр ресурсы, но не делать инструменты для редактирования.

Некоторые форматы файлов были проанализированы поверхностно - на уровне, достаточном для извлечения данных, но не более. Некоторые были проанализированы довольно хорошо, так что я даже смог реализовать не только извлечение данных, но и обратную упаковку.

Спустя некоторое время я погряз в дебрях ООП, так что на проектирование объектной структуры программы стало уходить столько времени, что я не мог довести до конца ни одной программы. Это было началом "творческой депрессии", в результате которой я надолго забросил программирование.

История
-------

В сентябре 2010 года я решил тряхнуть стариной и написать программу для распаковки и упаковки файлов с игровыми ресурсами. Чтобы не повторять прошлых ошибок, писать я решил на чистом C, без использования ООП. Я спроектировал плагинную систему и решил реализовать в этой программе поддержку только тех форматов, которые хорошо на неё ложатся.

К февралю 2011 года я практически завершил разработку программы, планируя добавить поддержку ещё одного формата (IWAD/PWAD). С этим форматом возникли сложности и работа застопорилась. Хотя я периодически вспоминал о программе и пытался доделать поддержку этого формата, в конце концов я нашёл какое-то более интересное занятие и забыл об этой программе.

Недавно я о ней вспомнил и подумал - не пропадать же добру, может кому пригодится. Попробовал собрать, исправил пару ошибок, из-за которых сборка не шла, удалил зачатки неудавшегося плагина и решил выложить всё это.

Поскольку к 2010 году я уже пользовался Debian'ом, то программа тоже делалась на нём. Сборка осуществляется с помощью shell-скрипта, для сборки нужны пакеты gcc, zlib1g, zlib1g-dev. Сборка в других системах не тестировалась.

Программа
---------

Программа называется PUP - Packer/UnPacker (слово pup также можно перевести с английского как "щенок"). Программа имеет несколько режимов работы:

1. Если программа запущена без опций, выводится справка по доступным опциям.
2. При указании опции --list программа выводит описание всех доступных плагинов.
3. При указании опции --unpack, программа попытается определить формат указанного ей файла и распакует его содержимое в указанный каталог, а метаданные файла сохранит в указанный файл. Если имя файла метаданных не указано, используется имя каталога с добавленным к нему расширением ".txt". Если имя каталога не указано, используется имя файла без расширения. Чтобы программа не пыталась определить формат файла самостоятельно, можно указать опцию --plugin с указанием определённого формата.
4. При указании опции --pack, программа запакует данные в указанный файл из указанного каталога с использованием метаданных из указанного файла. Если имя файла метаданных не указано, используется имя каталога с добавленным к нему расширением ".txt". Если имя каталога не указано, используется имя файла без расширения. При упаковке обязательно указать опцию --plugin с указанием формата формируемого файла.
5. При указании опции --savemeta, программа пытается определить формат указанного ей файла и сохранить метаданные в указанный файл. Если плагин не содержит метаданных, программа сообщит об этом и завершит работу. Если имя файла метаданных не указано, используется имя исходного файла с расширением, заменённым на ".txt". Чтобы программа не пыталась определить формат файла самостоятельно, можно указать опцию --plugin с указанием определённого формата.
6. При указании опции --print, программа выводит на стандартный вывод техническую информацию из каталога ресурсов указанного файла (смещение ресурса, его сжатый и исходный размер, название ресурса и т.п.). Чтобы программа не пыталась определить формат файла самостоятельно, можно указать опцию --plugin с указанием определённого формата.

Справка программы:

    $ ./pup
    Usage: pup --list
           pup --plugin <plugin> --pack <file> [<dir> [<meta>]]
           pup [--plugin <plugin>] --unpack <file> [<dir> [<meta>]]
           pup [--plugin <plugin>] --savemeta <file> [<meta>]
           pup [--plugin <plugin>] --print <file>
    Options:
           --plugin <plugin>   - specify certain plugin
    Modes:
           --list     - list of all supported plugins
           --pack <file> [<dir> [<meta>]]  - packing dir to specified file
           --unpack <file> [<dir> [<meta>]]  - unpacking specified file to dir
           --savemeta <file> [<meta>]  - only save metadata to specified metafile
           --print <file>    - print technical information to stdout

Список поддерживаемых форматов:

    $ ./pup --list
    Supported plugins:
    grp     GRP-files of Duke Nukem 3D, Witchaven, Redneck Rampage, Shadow Warrior
    gob     GOB-files of Star Wars: Dark Forces
    pak     PAK-files of Dune II: The Building of a Dynasty
    viv     VIV-files of the Need For the Speed 3: Hot Pursuit
    vpp     VPP-files of Red Faction, The Punisher, Summoner
    pack    PAK-files of Quake, Quake II, Half-Life, Heretic 2, MDK 2
    pack2   PAK-files of Daikatana
    wad2    WAD-file of Quake
    wad3    WAD-files of Half-Life
    res     RES-file of Comanche 3
    dpk4    DPK-file of Starmageddon 2
    dat     DAT-files of Fallout
    dat2    DAT-files of Fallout 2
    rff20   RFF-files of Blood, version 2.0
    rff30   RFF-files of Blood, version 3.0
    rff31   RFF-files of Blood, version 3.1

Программа отлично справляется с определением типа исходного файла, поэтому используемый плагин указывать совершенно не обязательно. Плагин нужно указывать только при упаковке, чтобы программа сформировала файл нужного вам формата.

Метаданные используются только в формате RFF. Метаданные - это дополнительные информационные поля, сопровождающие каждый упакованный файл. Эти поля никак нельзя восстановить по содержимому самого файла, поэтому при распаковке RFF-файла нужно сохранить эти поля в текстовый файл, а при упаковке - загрузить их из текстового файла.

Есть много различных упаковщиков и распаковщиков файлов игр. Уникальность моей программы заключается в поддержке форматов pak-файлов Daikatana, res-файлов Comanche 3 и rff-файлов Blood.

Формат pak-файлов Daikatana и его алгоритм компрессии я анализировал сам. Сам же написал декомпрессор, что было довольно легко, и компрессор, что оказалось значительно сложнее. Я не силён в алгоритмах сжатия, поэтому компрессор работает довольно медленно, однако сжимает он лучше, чем компрессор разработчиков. Распаковка и упаковка была реализована 15 сентября 2002, а более эффективная версия упаковщика - 20 октября 2002.

Формат res-файлов Comanche 3 я тоже анализировал сам. Сложность анализа этого формата заключалась в том, что имена ресурсов в нём были зашифрованы, так что просто глазами найти имена файлов оказалось непросто.

Тогда у меня была самописная программа, которая позволяла находить в произвольном файле содержащиеся в нём файлы определённых форматов по их сигнатурам. Например, wav-файлы и avi-файлы находились по сигнатуре RIFF (плюс дополнительные проверки), PCX-файлы находились по характерной для них сигнатуре и т.п. С помощью этой программы я нашёл в res-файле Comanche 3 точки начала нескольких файлов. Рядом с ними я увидел их размеры и какие-то смещения. И ещё были поля с непонятным содержимым. Я написал программу, которая выводила мне каталог всех ресурсов и смогла их распаковать в файлы, имена которых составлялись из порядкового номера ресурса.

Дальше я заметил в тех непонятных записях, что последние байты часто бывают одинаковыми и сделал предположение, что это - результат шифрования байта со значением 0 операцией XOR. Я подумал, что в конце имени файла обычно бывает его расширение и нашёл ресурсы, формат которых мне известен. Так я восстановил четырёхбайтовый ключ шифрования, который для шифрования 12-байтового имени файла использовался трижды. Первая программа была написана 09 февраля 2003 и умела только распаковывать ресурсы.

С rff-файлами Blood всё оказалось и проще и сложнее. Информацию об алгоритме шифрования файлов я нашёл в Интернете, однако она подходила не ко всем найденным мной файлам rff. Я нашёл несколько разных версий утилиты BARF для создания новых файлов для Blood и попробовал сформировать ими новые файлы. Так я нашёл три разных формата rff-файлов, распаковку и упаковку которых и реализовал. Однако, игра не захотела работать с родными файлами, обработанными распаковкой и упаковкой. В конце концов я реализовал сохранение и загрузку метаданных rff-файлов при их распаковке и упаковке соответственно. Приём сработал и Blood стал работать с пересобранными файлами.

С остальными форматами всё было проще. Некоторые я проанализировал сам, о некоторых почитал в интернете.

Архив с исходными текстами программы можно скачать здесь. Тут же есть текстовый файлик с именем ideas.txt, в котором я записывал идеи и отмечал этапы разработки программы.

(C) 2013 Владимир Ступин

Программа распространяется под лицензией GPL 3.