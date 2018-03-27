
LESSONS := lesson_1 \
           lesson_2
MAKE := 
#make_main
all: $(LESSONS)

lesson_1:
	make --directory=01-basic

lesson_2:
	make --directory=02-simple-drawing
