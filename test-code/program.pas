program minipas;

  var a,y: integer;
      b, i: integer;
      x: array [1..100] of real;




  function gcd (a, b: integer) : integer;
  begin
    while a*b <> 0 do
    begin
      if a > b then
        a := a-b
      else b := b-a;
    end;
    if a = 0 then
      gcd := b
    else gcd := a;
  end;


  function factorial (a: integer) : integer;
  var k, fact: integer;
  begin
    fact := 1;
    k := 2;
    while k <= a do
    begin
      fact := fact*k;
      k := k+1;
    end;
    factorial := fact;
  end;

  function sum (n: integer) : integer;
    var s, k: integer;
    begin
      s := 0;
      k := 1;
      while k <= n do
      begin
        sum := sum+k;
        k := k+1;
      end;
      if s <> n*(n+1)/2 then
        sum := -1
      else sum := s;
    end;
