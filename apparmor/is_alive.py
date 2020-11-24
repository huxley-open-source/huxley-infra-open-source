#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import requests
import smtplib
import threading
import time

def sendEmail(object):
	server = smtplib.SMTP_SSL('smtp.gmail.com', 465)
	server.ehlo()

	server.login('contato@thehuxley.com', '')

	subject = "Subject: %s Down" % object
	body = "Atenção, o %s caiu! Caso não seja restaurado, esta notificação continuará sendo enviada de hora em hora." % object

	message = "\r\n".join([
	  "From: contato@thehuxley.com",
	  "To: thehuxley@googlegroups.com",
	  subject,
	  "",
	  body
	])
	print message
	server.sendmail("contato@thehuxley.com", "thehuxley@googlegroups.com", message)

	server.quit()

def isEvaluatorDown():
	f = os.popen('sudo rabbitmqctl list_queues name consumers | grep "submission_queue\|oracle_queue"')

	result = f.read()

	queues = result.split('\n')

	del queues[-1]

	for line in queues:
		queue = line.split('\t')
		if int(queue[1]) <= 0:
			return True
	return False

def isHuxleyRestDown():
	try:
		response = requests.get("https://thehuxley.com/api/problems", timeout=3.000)
		return response.status_code != 200
	except requests.exceptions.Timeout as e:
	    return True

def checkEvaluator():
	while True:
		if (isEvaluatorDown()):
			sendEmail("Evaluator")
			print "Eva Down"
			time.sleep(3600)
		else:
			print "Eva Up"
			time.sleep(60)

def checkHuxleyRest():
	while True:
		if (isHuxleyRestDown()):
			sendEmail("Rest")
			print "Rest Down"
			time.sleep(3600)
		else:
			print "Rest Up"
			time.sleep(60)

try:
	t1 = threading.Thread(target=checkEvaluator, args=())
	t1.start()

	t2 = threading.Thread(target=checkHuxleyRest, args=())
	t2.start()
except Exception as e:	
	print "Erro: não foi possivel inciar a thread"
	print e
