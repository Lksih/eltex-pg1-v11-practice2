
**Сборка:**

```bash
make
```

**Добавление модуля в ядро:**

```bash
sudo insmod chardev.ko
```

![картинка](0.png)
![картинка](3.png)

**Удаление модуля из ядра:**

```bash
sudo rmmod chardev.ko
```

**Запись в файл (через `sudo su`):**

```bash
echo текст > /dev/chardev
```

**Чтение файла:**

```bash
cat /dev/chardev
```

![картинка](1.png)

**Информация о модуле:**

```bash
modinfo chardev.ko
```

![картинка](2.png)

**Удаление модуля:**

```bash
make clean
```