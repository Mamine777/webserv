const queryString = decodeURIComponent(process.env.QUERY_STRING);

const querylist = queryString.split("&");
let queryMap = new Map();
querylist.forEach((val) => {
	const temparr = val.split("=", 2);
	queryMap[temparr[0]] = temparr[1];
});


function isNumber(str) {
    return !isNaN(Number(str)) && str.trim() !== '';
}

if (queryMap["n1"] !== undefined && queryMap["n2"] !== undefined && queryMap["op"] !== undefined) {

	let res;
	const n1 = Number(queryMap["n1"])
	const n2 = Number(queryMap["n2"])
	const op = queryMap["op"]
	if (op == "+")
		res = n1 + n2;
	if (op == "-")
		res = n1 - n2;
	if (op == "*")
		res = n1 * n2;
	if (op == "/")
		res = n1 / n2;
	if (!isNumber(queryMap["n1"]))
		res = "Number 1 is not a number";
	else if (!isNumber(queryMap["n2"]))
		res = "Number 2 is not a number";
	else if (!["+", "-", "/", "*"].includes(queryMap["op"]))
		res = "Operand has to be one of these: +, -, /, *";

	console.log(`Status: 200 OK\r
	Content-Type: text/html\r
	Connection: close\r
	\r\n\r
	<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Simple Calculator</title>
	</head>
	<body>
		<h1>Look at my simple calculator</h1>
		<form action="/cgi/calc.js" method="get">
			<label for="n1">Number 1</label>
			<input type="text" name="n1" id="n1" value="${n1}">
			<label for="op">Opperand</label>
			<input type="text" name="op" id="op" value="${op}" placeholder="+, -, /, *">
			<label for="n2">Number 2</label>
			<input type="text" name="n2" id="n2" value="${n2}">
			<input type="submit">
		</form>
		<p>Result: ${res}</p>
	</body>
	</html>
	`)
} else {
	console.log(`Status: 200 OK\r
	Content-Type: text/html\r
	Connection: close\r
	\r\n\r
	<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Simple Calculator</title>
	</head>
	<body>
		<h1>Look at my simple calculator</h1>
		<form action="/cgi/calc.js" method="get">
			<label for="n1">Number 1</label>
			<input type="text" name="n1" id="n1">
			<label for="op">Opperand</label>
			<input type="text" name="op" id="op"placeholder="+, -, /, *">
			<label for="n2">Number 2</label>
			<input type="text" name="n2" id="n2">
			<input type="submit">
		</form>
		<p>Result: </p>
	</body>
	</html>
	`)
}
