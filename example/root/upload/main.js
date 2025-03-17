const inputButton = document.getElementById("inputButton");

console.log(inputButton)

let header = document.getElementsByClassName("header")[0];

header.addEventListener("click", () => {
	window.location = "/"
})

inputButton.onclick = () => {
	document.getElementById("fileinput").click();
}

document.getElementById("fileinput").addEventListener("change", async (event) => {
	const file = event.target.files[0];
	if (!file) {
		alert("please select a real file!");
		return ;
	}

	try {
		const response = await fetch(`http://${window.location.host}/uploads/${file.name}`, {
			method: "POST",
			headers: {
				"Content-Type": file.type || "application/octet-stream",
				"Content-Size": file.size
			},
			body: file
		});
		const result = await response.text();
		window.location = `http://${window.location.host}/uploads/${file.name}`;
	} catch (error) {
		console.log(error);
	}
})

document.getElementById("deleteButton").addEventListener("click", async 	() => {
	const filename = document.getElementById("deleteInput").value;
	console.log(filename);
	
	if (!filename || filename === "") {
		alert("please enter a file");
		return; 
	}

	try {
		const response = await fetch(`http://${window.location.host}/uploads/${filename}`, {
			method: "DELETE"
		});
		const result = await response.text();
		console.log(result)
	} catch (error) {
		console.log(error);
	}
})