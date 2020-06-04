<?php
/******************************************************************************/
/**************************** MY PROPOSAL EDIT FORM ******************************/
/******************************************************************************/
function arduino_projects_blog_my_proposal_edit_form($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	/* get current proposal */
	$proposal_id = (int) arg(3);
	//$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE id = %d", $proposal_id);
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	$proposal_data = $proposal_q->fetchObject();
	/*if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		/*} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('arduino-projects/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('arduino-projects/manage-proposal');
		return;
	}*/
	$user_data = user_load($proposal_data->uid);
	$form['contributor_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name of the Contributor'),
		'#size' => 350,
		'#maxlength' => 350,
		'#default_value' => $proposal_data->contributor_name,
		'#disabled' => TRUE
	);
	$form['student_email_id'] = array(
		'#type' => 'item',
		'#title' => t('Email'),
		'#markup' => $user_data->mail
	);
	$form['university'] = array(
		'#type' => 'textfield',
		'#title' => t('University/Institute/Organisation'),
		'#size' => 200,
		'#maxlength' => 200,
		'#default_value' => $proposal_data->university,
		'#disabled' => TRUE
	);
	$form['position'] = array(
		'#type' => 'textfield',
		'#title' => t('Position'),
		'#size' => 80,
		'#maxlength' => 200,
		'#disabled' => TRUE,
		'#default_value' => $proposal_data->position
	);
	$form['how_did_you_hear_about_us'] = array(
		'#type' => 'textfield',
		'#title' => t('How did you hear about us?'),
		'#default_value' => $proposal_data->how_did_you_hear_about_us,
		'#disabled' => TRUE
	);
	if($proposal_data->country == 'India'){
	$form['country'] = array(
		'#type' => 'select',
		'#title' => t('Country'),
		'#options' => array(
			'India' => 'India',
			'Others' => 'Others'
		),
		'#default_value' => $proposal_data->country,
		'#tree' => TRUE,
		'#validated' => TRUE,
		'#disabled' => TRUE
	);
	$form['all_state'] = array(
		'#type' => 'select',
		'#title' => t('State'),
		'#options' => _df_list_of_states(),
		'#default_value' => $proposal_data->state,
		'#validated' => TRUE,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'India'
				)
			)
		),
		'#disabled' => TRUE
	);
	$form['city'] = array(
		'#type' => 'select',
		'#title' => t('City'),
		'#options' => _df_list_of_cities(),
		'#default_value' => $proposal_data->city,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'India'
				)
			)
		),
		'#disabled' => TRUE
	);
}
else{ 
	$form['other_country'] = array(
		'#type' => 'textfield',
		'#title' => t('Country(Other than India)'),
		'#size' => 100,
		'#default_value' => $proposal_data->country,
		'#attributes' => array(
			'placeholder' => t('Enter your country name')
		),
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		),
		'#disabled' => TRUE
	);
	$form['other_state'] = array(
		'#type' => 'textfield',
		'#title' => t('State(Other than India)'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your state/region name')
		),
		'#default_value' => $proposal_data->state,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		),
		'#disabled' => TRUE
	);
	$form['other_city'] = array(
		'#type' => 'textfield',
		'#title' => t('City(Other than India)'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your city name')
		),
		'#default_value' => $proposal_data->city,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		),
		'#disabled' => TRUE
	);
}
	
	$form['pincode'] = array(
		'#type' => 'textfield',
		'#title' => t('Pincode'),
		'#size' => 30,
		'#maxlength' => 6,
		'#default_value' => $proposal_data->pincode,
		'#attributes' => array(
			'placeholder' => 'Insert pincode of your city/ village....'
		),
		'#disabled' => TRUE
	);
	$form['project_title'] = array(
		'#type' => 'textarea',
		'#title' => t('Title of the Project'),
		'#size' => 300,
		'#maxlength' => 350,
		'#default_value' => $proposal_data->project_title,
		'#disabled' => TRUE
	);
	$form['abstract'] = array(
		'#type' => 'textarea',
		'#title' => t('Abstract'),
		'#size' => 600,
		'#maxlength' => 600,
		'#required' => TRUE,
		'#default_value' => $proposal_data->abstract
	);
	$form['reference'] = array(
		'#type' => 'textarea',
		'#title' => t('Reference'),
		'#size' => 10000,
		'#default_value' => $proposal_data->reference,
		'#required' => TRUE
	);
	$form['uplpoaded_images'] = array(
            '#markup' => '<h5><p> Edit existing reference images</p></h5>(Select Check box to delete existing photos)',
            '#prefix' => '<div id="uploaded_images"><table><tr>',
            '#suffix' => ''
        );
	 $query1 = db_select('arduino_projects_blog_proposal_images');
        $query1->fields('arduino_projects_blog_proposal_images');
        $query1->condition('proposal_id', $proposal_id);
        $result1 = $query1->execute();
        while ($row1 = $result1->fetchObject()) {
            $form['imagecheck@' . $row1->id] = array(
                '#type' => 'checkbox',
                '#field_suffix' => '<img style="width:500px; padding-left :20px;height:350px" src=' . $GLOBALS['base_url'] . "/project_uploads/" . $row1->path . ' />'
            );
        }
    $form['enduploadphotos'] = array(
            '#markup' => '',
            '#prefix' => '',
            '#suffix' => '</tr></table></div>'
        );
    $form['reference_files'] = array(
            '#type' => 'fieldset',
            '#tree' => TRUE,
            '#prefix' => '<div id="names-fieldset-wrapper">',
            '#suffix' => '</div>',
            '#title' => t('Upload Reference images'),
        );
        if (empty($form_state['num_names'])) {
            $form_state['num_names'] = 1;
        }
        for ($i = 0; $i < $form_state['num_names']; $i++) {
        	$temp_no = $i;
            $form['reference_files'][$i]['image'] = array(
                //'#title' => t('Add Event Image'),
                '#type' => 'file',
                '#weight' => '5',
                '#description' => t('Upload filenames with allowed extensions only. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions: ') . variable_get('reference_image_extensions', '') . '</span>',
                // We need this to know which file element this is.
                // By default drupal would name all as files[reference_files]
                '#name' => 'files[reference_files_' . $i . '_name]'
            );
            $form['reference_files']["images_count"] = array(
				"#type" => "hidden",
				"#value" => $temp_no
			);
        }
        if($i < 5){
        $form['reference_files']['add_name'] = array(
            '#type' => 'submit',
            '#value' => t('Add Image'),
            '#limit_validation_errors' => array(),
            '#submit' => array(
                'reference_files_add_more_add_one'
            ),
            '#ajax' => array(
                'callback' => 'reference_files_add_more_callback',
                'wrapper' => 'names-fieldset-wrapper'
            )
        );
    }
        if ($form_state['num_names'] > 1) {
            $form['reference_files']['remove_name'] = array(
                '#type' => 'submit',
                '#value' => t('Remove Image'),
                '#limit_validation_errors' => array(),
                '#submit' => array(
                    'reference_files_add_more_remove_one'
                ),
                '#ajax' => array(
                    'callback' => 'reference_files_add_more_callback',
                    'wrapper' => 'names-fieldset-wrapper'
                )
            );
        }
        if ($no_js_use) {
            if (!empty($form['reference_files']['remove_name']['#ajax'])) {
                unset($form['reference_files']['remove_name']['#ajax']);
            }
            unset($form['reference_files']['add_name']['#ajax']);
        }
    $form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	$form['cancel'] = array(
		'#type' => 'item',
		'#markup' => l(t('Cancel'), 'arduino-projects/my-proposals')
	);
	return $form;
}
function reference_files_add_more_callback($form, $form_state) {
    return $form['reference_files'];
}


function reference_files_add_more_add_one($form, &$form_state) {
    $form_state['num_names']++;
    $form_state['rebuild'] = TRUE;
    //$form_state['no_redirect'] = TRUE;
}


function reference_files_add_more_remove_one($form, &$form_state) {
    if ($form_state['num_names'] > 1) {
        $form_state['num_names']--;
    }
    $form_state['rebuild'] = TRUE;
}
function arduino_projects_blog_my_proposal_edit_form_submit($form, &$form_state)
{
	global $user;
	/* get current proposal */
	$proposal_id = (int) arg(3);
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('arduino-projects/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('arduino-projects/manage-proposal');
		return;
	}
	/* update proposal */
	$v = $form_state['values'];
	$query = "UPDATE arduino_projects_blog_proposal SET 
				abstract=:abstract,
				reference=:reference,
				approval_status = :approval_status
				WHERE id=:proposal_id";
	$args = array(
		':abstract' => $v['abstract'],
		':reference' => $v['reference'],
		':approval_status' => 0,
		':proposal_id' => $proposal_id
	);
	$result = db_query($query, $args);
	$deletecounter = 0;
    $query_img = db_select('arduino_projects_blog_proposal_images');
    $query_img->fields('arduino_projects_blog_proposal_images');
    $query_img->condition('proposal_id', $proposal_id);
    $result_img = $query_img->execute();
  	$root_path= arduino_projects_blog_files_path();
        while ($row_img = $result_img->fetchObject()) {
            if ($form_state['values']['imagecheck@' . $row_img->id] == 1) {
                if (file_exists($root_path . $row_img->path)) {
                    unlink($root_path .$row_img->path);
                    $query2 = db_delete('arduino_projects_blog_proposal_images');
                    $query2->condition('id', $row_img->id);
                    $delete_img = $query2->execute();
                    if ($delete_img != 0) {
                        $deletecounter++;
                    }
                } 
                else {
                    drupal_set_message(t('Error Could not delete :') . $filename . t(', file does not exist'), 'error');
                }
            }
        }
        /* For adding more images to existing event */
        $items = array();
        $root_path = arduino_projects_blog_files_path();
        $query = db_select('arduino_projects_blog_proposal');
        $query->fields('arduino_projects_blog_proposal');
        $query->condition('id', $proposal_id);
        $result = $query->execute();
        $row_folder = $result->fetchObject();
        //var_dump($row_folder);die;
        $eventfolder = $row_folder->directory_name . '/proposal_images/';
        $dest_path = $eventfolder;
        if (!is_dir($root_path . $dest_path)) {
            mkdir($root_path . $dest_path);
        }
  $imageupload = 0;
        /* uploading files */
        foreach ($_FILES['files']['name'] as $file_form_name => $file_name) {
            if ($file_name) {
                if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name])) {
                    drupal_set_message(t("Error uploading file. File !filename already exists."), array(
                        '!filename' => $_FILES['files']['name'][$file_form_name]
                    ), 'error');
                    return;
                }
                
                /* uploading file */
                else if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name])) {
                    $imagequery = "INSERT INTO arduino_projects_blog_proposal_images(proposal_id,name,path) VALUES
						  (:proposal_id,:name, :path)";
                    $imageargs = array(
                        ":proposal_id" => $proposal_id,
                        ":name" => $_FILES['files']['name'][$file_form_name],
                        ":path" => $dest_path . $_FILES['files']['name'][$file_form_name]
                    );
                    /* storing the row id in $result */
                    $imageresult = db_query($imagequery, $imageargs, array(
                        'return' => Database::RETURN_INSERT_ID
                    ));
                    if ($imageresult != 0) {
                        $imageupload++;
                    }
                    drupal_set_message($file_name . ' uploaded successfully.', 'status');
                } 
                else {
                    drupal_set_message($file_name . $dest_path . t('was having an error while uploading :') , 'error');
                }
            }
        }
        if (!$result && $imageupload == 0 && $deletecounter == 0) {
            drupal_set_message(t("Something went wrong, please try again."), "error");
        } 
        else {
            if ($imageupload != 0) {
                drupal_set_message(t("Event's Pictures are added successfully"), "status");
            }
            if ($deletecounter != 0) {
                drupal_set_message(t("Event's Pictures are deleted successfully"), "status");
            }
        }
     
	drupal_set_message(t('Proposal Updated'), 'status');
	$user_data = user_load($proposal_data->uid);
		$email_to = $user_data->mail;
		$from = variable_get('arduino_projects_blog_from_email', '');
		$bcc = $user->mail . ', ' . variable_get('arduino_projects_blog_bcc_emails', '');
		$cc = variable_get('arduino_projects_blog_cc_emails', '');
		$params['arduino_projects_blog_proposal_resubmit_ack']['proposal_id'] = $proposal_data->id;
		$params['arduino_projects_blog_proposal_resubmit_ack']['user_id'] = $proposal_data->uid;
		$params['arduino_projects_blog_proposal_resubmit_ack']['headers'] = array(
			'From' => $from,
			'MIME-Version' => '1.0',
			'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
			'Content-Transfer-Encoding' => '8Bit',
			'X-Mailer' => 'Drupal',
			'Cc' => $cc,
			'Bcc' => $bcc
		);
		if (!drupal_mail('arduino_projects_blog', 'arduino_projects_blog_proposal_resubmit_ack', $email_to, language_default(), $params, $from, TRUE))
			drupal_set_message('Error sending email message.', 'error');
	drupal_goto('arduino-projects/my-proposals');
}

function arduino_projects_blog_my_proposal_view_form($form, &$form_state)
{
	global $user;
	$root_path = arduino_projects_blog_files_path();
	/* get current proposal */
	$proposal_id = (int) arg(3);
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	if ($proposal_q)
	{
		if ($proposal_data = $proposal_q->fetchObject())
		{
			/* everything ok */
		} //$proposal_data = $proposal_q->fetchObject()
		else
		{
			drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
			drupal_goto('arduino-projects/manage-proposal');
			return;
		}
	} //$proposal_q
	else
	{
		drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
		drupal_goto('arduino-projects/manage-proposal');
		return;
	}
	if ($proposal_data->contact_no == "NULL" || $proposal_data->contact_no == "")
	{
		$contact_no = "Not Entered";
	} //$proposal_data->project_guide_name == NULL
	else
	{
		$contact_no = $proposal_data->contact_no;
	}
	$form['contributor_name'] = array(
		'#type' => 'item',
		'#markup' => l($proposal_data->contributor_name, 'user/' . $proposal_data->uid),
		'#title' => t('Contributor name')
	);
	$form['student_email_id'] = array(
		'#title' => t('Student Email'),
		'#type' => 'item',
		'#markup' => user_load($proposal_data->uid)->mail,
		'#title' => t('Email')
	);
	$form['contributor_contact_no'] = array(
		'#title' => t('Contact No.'),
		'#type' => 'item',
		'#markup' => $contact_no,
	);
	$form['university'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->university,
		'#title' => t('University/Institute')
	);
	$form['position'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->position,
		'#title' => t('Position')
	);
	$form['country'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->country,
		'#title' => t('Country')
	);
	$form['all_state'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->state,
		'#title' => t('State')
	);
	$form['city'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->city,
		'#title' => t('City')
	);
	$form['pincode'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->pincode,
		'#title' => t('Pincode/Postal code')
	);
	$form['how_did_you_hear_about_us'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->how_did_you_hear_about_us,
		'#title' => t('How did you hear about us')
	);
	$form['project_title'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->project_title,
		'#title' => t('Title of the Project')
	);
	$form['abstract'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->abstract,
		'#title' => t('Abstract')
	);
	$form['reference'] = array(
		'#type' => 'item',
		'#markup' => $proposal_data->reference,
		'#title' => t('References')
	);
	/*$form['reference_images'] = array(
		'#type' => 'item',
		'#title' => t('Reference Images'),
		'#markup' => l('Download Reference Images', 'arduino-projects/download/reference-images/' . $proposal_id)
	);*/
	$form['creation_date'] = array(
		'#type' => 'item',
		'#title' => t('Submission date'),
		'#markup' => date('d-m-Y', $proposal_data->creation_date)
	);
	$form['expected_date_of_completion'] =array(
		'#type' => 'item',
		'#title' => t('Expected Completion date'),
		'#markup' => date('d-m-Y', $proposal_data->expected_date_of_completion)
	);
	if($proposal_data->resubmit_comment){
	$form['resubmit_comment'] = array(
		'#type' => 'item',
		'#title' => t('Comment from the Reviewer'),
		'#markup' => $proposal_data->resubmit_comment
	);
	}
	$form['cancel'] = array(
		'#type' => 'item',
		'#markup' => l(t('Cancel'), 'arduino-projects/my-proposals')
	);
	return $form;
}