program test;

type 
  newt = integer; 
  newnewt = real;

var 
  x,y,t,q : integer;
  z : real; 
  sh : string;

const 
  k = 5; 
  h = 1890.03E+08;
  sc = 'Йа строковое константо';

{
	Системные процедуры, о которых наш компилятор пока не знает
}

procedure writeln;
begin
end;

procedure readln;
begin
end;

procedure some_proc;
begin
end;

procedure noneproc(zetta : integer);
begin
end;

procedure debug_w(a : integer);
begin
end;

function super_puper_func(var a : real; c,d,e,p : integer) : integer;
var
  NewVar : integer;

function inner_func:integer;
begin
c := 1;
end;

begin
noneproc(e);
{
if true then
	x := 0
else x := 1;}
{super_puper_func := 0;}
NewVar := 18;
end;

procedure never_return;
begin
{NewVar := 0;}
x := 0;
end;

begin
z := 56.0;
y := 546;
debug_w(y);
x := 1;
debug_w(x);
// writeln(z);

{	Ветвление	}
{if TRUE then
	begin
		X := 1;
		y := 2;
	end
else
}
{	Вызов функции	}
x := super_puper_func(z, y, x, 1, 2);

{	Всяческие присваивания	}
// z := 5;
// t := 0.5;
sh := sc;
sh := 'Это просто строка';
{	Вызов процедуры	}
// some_proc;

{	Цикл while	}
{while true do begin
	x := super_puper_func(x, y, z, 1);
end;}

{	Цикл repeat	}
{repeat
	y := 0;
	x := y;
until false;}

// k := 5;
// writeln;
sh := 'Новая строка';
end.
