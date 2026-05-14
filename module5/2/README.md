
**Сборка:**

```bash
make
```

**Добавление модуля в ядро:**

```bash
sudo insmod proc.ko
```

**Удаление модуля из ядра:**

```bash
sudo rmmod proc.ko
```

**Запись в файл (через `sudo su`):**

```bash
echo текст > /proc/proc
```

**Чтение файла:**

```bash
cat /proc/proc
```

![картинка](1.png)

**Информация о модуле:**

```bash
modinfo proc.ko
```

![картинка](2.png)

**Удаление модуля:**

```bash
make clean
```