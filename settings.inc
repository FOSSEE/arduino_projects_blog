<?php
// $Id$
function arduino_projects_blog_settings_form($form, $form_state)
{
	$form['bcc_emails'] = array(
		'#type' => 'textfield',
		'#title' => t('(Bcc) Notification emails'),
		'#description' => t('Specify emails id for Bcc option of mail system with comma separated'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('arduino_projects_blog_bcc_emails', '')
	);
	$form['cc_emails'] = array(
		'#type' => 'textfield',
		'#title' => t('(Cc) Notification emails'),
		'#description' => t('Specify emails id for Cc option of mail system with comma separated'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('arduino_projects_blog_cc_emails', '')
	);
	$form['from_email'] = array(
		'#type' => 'textfield',
		'#title' => t('Outgoing from email address'),
		'#description' => t('Email address to be display in the from field of all outgoing messages'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('arduino_projects_blog_from_email', '')
	);
	$form['extensions']['reference_image'] = array(
		'#type' => 'textfield',
		'#title' => t('Allowed file extensions for uploading resource files'),
		'#description' => t('A comma separated list WITHOUT SPACE of source file extensions that are permitted to be uploaded on the server'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('reference_image_extensions', '')
	);
	$form['extensions']['step_image_extensions'] = array(
		'#type' => 'textfield',
		'#title' => t('Allowed file extensions for uploading step images'),
		'#description' => t('A comma separated list WITHOUT SPACE of source file extensions that are permitted to be uploaded on the server'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('step_image_extensions', '')
	);
	$form['no_of_images_allowed_project_submission'] = array(
		'#type' => 'textfield',
		'#title' => t('Enter the number of images that can be uploaded during project submission'),
		'#size' => 50,
		'#default_value' => variable_get('no_of_images_allowed_project_submission', '')
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	return $form;
}
function arduino_projects_blog_settings_form_validate($form, &$form_state)
{
	return;
}
function arduino_projects_blog_settings_form_submit($form, &$form_state)
{
	variable_set('arduino_projects_blog_bcc_emails', $form_state['values']['bcc_emails']);
	variable_set('arduino_projects_blog_cc_emails', $form_state['values']['cc_emails']);
	variable_set('arduino_projects_blog_from_email', $form_state['values']['from_email']);
	variable_set('reference_image_extensions', $form_state['values']['reference_image']);
	variable_set('step_image_extensions', $form_state['values']['step_image_extensions']);
	variable_set('no_of_images_allowed_project_submission', $form_state['values']['no_of_images_allowed_project_submission']);
	drupal_set_message(t('Settings updated'), 'status');
}
