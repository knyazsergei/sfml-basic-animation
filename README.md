# README #

Для работы требуется наличие sfml

Пространство имен Ani
Класс Animation содержит пошаговые трансформации над объектами текста и фигур.
Класс Animations имеет 2 метода:
1)Метод add принимает в качестве параметров <объект>, <тип анимации>, <параметр анимации>, <продолжительность>, <задержка>
возможные объекты: тест, фигура, спрайт
возможные типы анимации: zoom, left, right, top, boеttom, rotate, opacity
возможные парметры анимации: коэфицент масштабирования, расстояние сдвига в пикселях, угол поворота в градусах, степен прозрачности [0..255]
2)Метод play обновляет состояние объектов, в качестве параметра передается пройденое время.

Остальные классы нужны только для демонстрации.
