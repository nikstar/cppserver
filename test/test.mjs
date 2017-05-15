<!doctype html>
<title>Script</title>

<h1>Hello from mjs server!</h1>

<p>And here goes code:

<p><code>
<?
function fact(n) {
    if n == 0 {
        return 1;
    }
    else {
        return n * fact(n - 1);
    }
}
var f = fact(5);
write(f);
?> 
</code>

<p>The end.