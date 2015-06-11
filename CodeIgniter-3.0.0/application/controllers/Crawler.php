<?php
defined('BASEPATH') OR exit('No direct script access allowed');

class Crawler extends CI_Controller {

	public function index()
	{
		//Load webpage which takes input
		$this->load->view('Query');
	}

	//'Query' view will send input data back to this controller
	//This function will send input to the c++ program 'crawl_server' 
	//and receive the corresponding schedule.
	//Finally it'll load another view named 'result' which will display response to 
	//the user.
	public function process(){
		//capturing input
		$source = $this->input->post('src');
		$destination = $this->input->post('dest');

		//Getting Json output from the c++ program using sockets.
		error_reporting(E_ALL);
		$port = 4001;
		$address = "localhost";

		$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($socket === false){
			echo "Failed: ". socket_strerror(socket_last_error($socket))."\n";
		}

		// echo "Attempting to connect to '$address' on port '$port'...'\n";
		$result = socket_connect($socket, $address, $port);

		if ($result === false){
			echo "Failed: ". socket_strerror(socket_last_error($socket))."\n";
		}

		//request data to be sent to the server
		$request = $source . " " . $destination . " ";
		$req = $request;
		$response = '';

		// echo "Sending...\n";  
		socket_send($socket, $req, strlen($req), MSG_WAITALL);
		// echo "OK.\n";

		// echo "Reading response:\n\n";
		while ($out = socket_read($socket, 2048)) {
			if(is_null($out))
				break;
			$response = $response . $out;
		}

		//$data contains final response.
		$data['res'] = $response;

		socket_close($socket);

		//loading 'result' view and send $data to it.
		$this->load->view('result', $data);

	}
}
