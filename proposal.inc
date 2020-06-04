<?php
// $Id$
/*
Approval Status :
0 - Pending
1 - Approved
4 - Dis-Approved
2 - Resubmit proposal
3 - Completed
Solution Status :
0 - Pending
1 - Approved
2 - Dis-Approved
Solution Display :
0 - No
1 - Yes
*/
function arduino_projects_blog_proposal_form($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$root_path = arduino_projects_blog_files_path();
	/************************ start approve book details ************************/
	if ($user->uid == 0)
	{
		$msg = drupal_set_message(t('It is mandatory to ' . l('login', 'user') . ' on this website to access the proposal form. If you are new user please create a new account first.'), 'error');
		//drupal_goto('/pssp');
		drupal_goto('user/login', array('query' => drupal_get_destination()));
		return $msg;
	} //$user->uid == 0
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('uid', $user->uid);
	$query->orderBy('id', 'DESC');
	$query->range(0, 1);
	$proposal_q = $query->execute();
	$proposal_data = $proposal_q->fetchObject();
	if ($proposal_data)
	{
		if ($proposal_data->approval_status == 0 || $proposal_data->approval_status == 1 || $proposal_data->approval_status == 2)
		{
			drupal_set_message(t('We have already received your proposal.'), 'status');
			drupal_goto('arduino-projects');
			return;
		} //$proposal_data->approval_status == 0 || $proposal_data->approval_status == 1
	} //$proposal_data
	$imp = t('<span style="color: red;">*This is a mandatory field</span>');
	$form['#attributes'] = array(
		'enctype' => "multipart/form-data"
	);
	$form['contributor_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name of the contributor'),
		'#size' => 350,
		'#attributes' => array(
			'placeholder' => t('Enter your full name.....')
		),
		'#maxlength' => 350,
		'#required' => TRUE,
	);
	$form['contributor_contact_no'] = array(
		'#type' => 'textfield',
		'#title' => t('Contact No.'),
		'#size' => 10,
		'#attributes' => array(
			'placeholder' => t('Enter your contact number')
		),
		'#maxlength' => 250
	);
	$form['contributor_email_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Email'),
		'#size' => 30,
		'#value' => $user->mail,
		'#disabled' => TRUE
	);
	$form['university'] = array(
		'#type' => 'textfield',
		'#title' => t('University/Institute/Organisation'),
		'#size' => 80,
		'#maxlength' => 200,
		'#required' => TRUE,
	);
	$form['position'] = array(
		'#type' => 'textfield',
		'#title' => t('Position'),
		'#size' => 80,
		'#maxlength' => 200,
	);
	$form['country'] = array(
		'#type' => 'select',
		'#title' => t('Country'),
		'#options' => array(
			'India' => 'India',
			'Others' => 'Others'
		),
		'#required' => TRUE,
		'#tree' => TRUE,
		'#validated' => TRUE
	);
	$form['other_country'] = array(
		'#type' => 'textfield',
		'#title' => t('Other than India'),
		'#size' => 100,
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
		'#description' => $imp,
	);
	$form['other_state'] = array(
		'#type' => 'textfield',
		'#title' => t('State other than India'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your state/region name')
		),
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		),
		'#description' => $imp,
	);
	$form['other_city'] = array(
		'#type' => 'textfield',
		'#title' => t('City other than India'),
		'#size' => 100,
		'#attributes' => array(
			'placeholder' => t('Enter your city name')
		),
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'Others'
				)
			)
		),
		'#description' => $imp,
	);
	$form['all_state'] = array(
		'#type' => 'select',
		'#title' => t('State'),
		'#options' => _df_list_of_states(),
		'#validated' => TRUE,
		'#states' => array(
			'visible' => array(
				':input[name="country"]' => array(
					'value' => 'India'
				)
			)
		),
		'#description' => $imp
	);
	$form['city'] = array(
            '#type' => 'select',
            '#title' => t('City'),
            '#options' => _df_list_of_cities(),
            '#validated' => TRUE,
            '#states' => array(
                'visible' => array(
                    ':input[name="country"]' => array(
                        'value' => 'India'
                    )
                )
            ),
            '#description' => $imp,
	);
	$form['pincode'] = array(
		'#type' => 'textfield',
		'#title' => t('Pincode'),
		'#size' => 6
	);
	$form['how_did_you_hear_about_us'] = array(
		'#type' => 'select',
		'#title' => t('How did you hear about us?'),
		'#options' => array(
			'FOSSEE website' => 'FOSSEE website',
			'Spoken-tutorial website' => 'Spoken-tutorial website',
			'Attended the workshop' => 'Attended the workshop',
			'Recommended by friend or family' => 'Recommended by friend or family'
		),
	);
	
	/***************************************************************************/
	$form['hr'] = array(
		'#type' => 'item',
		'#markup' => '<hr>'
	);
	$form['project_title'] = array(
		'#type' => 'textarea',
		'#title' => t('Title of the Project'),
		'#size' => 255,
		'#description' => t('Maximum character limit is 255, minimum character 10'),
		'#required' => TRUE
	);
	$form['abstract'] = array(
		'#type' => 'textarea',
		'#title' => t('Abstract'),
		'#size' => 600,
		'#description' => t('Minimum character limit is 300'),
		'#required' => TRUE
	);	
	$form['reference'] = array(
		'#type' => 'textarea',
		'#title' => t('References'),
		'#size' => 500,
		'#required' => TRUE,
		'#attributes' => array(
			'placeholder' => 'The links to the documents or websites which are referenced while proposing this project.'
		)
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
                '#name' => 'files[reference_files_' . $i . '_name]',
                //'#attributes' => array('multiple' => 'multiple'),
                //'#upload_location' => 'public://images/proposal_images',
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
    $form['expected_date_of_completion'] = array(
    	'#type' => 'textfield',
    	'#title' => 'Expected completion date',
    	'#disabled' => TRUE,
    	'#value' => date('d-m-Y', strtotime("+1 months", strtotime("NOW")))
    );
	$form['term_condition'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Terms And Conditions'),
		'#options' => array(
			'status' => t('<a href="/arduino-projects/terms-and-conditions" target="_blank">I agree to the Terms and Conditions</a>')
		),
		'#required' => TRUE
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
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

function arduino_projects_blog_proposal_form_validate($form, &$form_state)
{
	$project_title = $form_state['values']['project_title'];
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('project_title', $project_title);
	$query->condition(db_or()->condition('approval_status',1)->condition('approval_status',3)); 
	$result = $query->execute()->rowCount();
	//var_dump($result);die;
	if ($result >= 1)
	{
		form_set_error('project_title', t('Project title name already exists'));
		return;
	}
	if ($form_state['values']['term_condition'] == '1')
	{
		form_set_error('term_condition', t('Please check the terms and conditions'));
		// $form_state['values']['country'] = $form_state['values']['other_country'];
	} //$form_state['values']['term_condition'] == '1'
	if ($form_state['values']['country'] == 'Others')
	{
		if ($form_state['values']['other_country'] == '')
		{
			form_set_error('other_country', t('Enter country name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['other_country'] == ''
		else
		{
			$form_state['values']['country'] = $form_state['values']['other_country'];
		}
		if ($form_state['values']['other_state'] == '')
		{
			form_set_error('other_state', t('Enter state name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['other_state'] == ''
		else
		{
			$form_state['values']['all_state'] = $form_state['values']['other_state'];
		}
		if ($form_state['values']['other_city'] == '')
		{
			form_set_error('other_city', t('Enter city name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['other_city'] == ''
		else
		{
			$form_state['values']['city'] = $form_state['values']['other_city'];
		}
	} //$form_state['values']['country'] == 'Others'
	else
	{
		if ($form_state['values']['country'] == '0')
		{
			form_set_error('country', t('Select country name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['country'] == ''
		if ($form_state['values']['all_state'] == '0')
		{
			form_set_error('all_state', t('Select state name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['all_state'] == ''
		if ($form_state['values']['city'] == '0')
		{
			form_set_error('city', t('Select city name'));
			// $form_state['values']['country'] = $form_state['values']['other_country'];
		} //$form_state['values']['city'] == ''
	}
	//Validation for project title
	$form_state['values']['project_title'] = trim($form_state['values']['project_title']);
	if ($form_state['values']['project_title'] != '')
	{
		if (strlen($form_state['values']['project_title']) > 255)
		{
			form_set_error('project_title', t('Maximum charater limit is 255 charaters only, please check the length of the project title'));
		} //strlen($form_state['values']['project_title']) > 250
		else if (strlen($form_state['values']['project_title']) < 10)
		{
			form_set_error('project_title', t('Minimum charater limit is 10 charaters, please check the length of the project title'));
		} //strlen($form_state['values']['project_title']) < 10
	} //$form_state['values']['project_title'] != ''
	else
	{
		form_set_error('project_title', t('Project title shoud not be empty'));
	}
	$form_state['values']['abstract'] = trim($form_state['values']['abstract']);
	if ($form_state['values']['abstract'] != '')
	{
		if (strlen($form_state['values']['abstract']) > 500)
		{
			form_set_error('abstract', t('Maximum charater limit is 500 charaters only, please check the length of the description'));
		} //strlen($form_state['values']['project_title']) > 250
		else if (strlen($form_state['values']['abstract']) < 300)
		{
			form_set_error('abstract', t('Minimum charater limit is 300 charaters, please check the length of the description'));
		} //strlen($form_state['values']['project_title']) < 10
	} //$form_state['values']['project_title'] != ''
	else
	{
		form_set_error('description', t('Abstract shoud not be empty'));
	}

if (isset($_FILES['files']))
	{
		/* check if atleast one source or result file is uploaded */
		/*if (!($_FILES['files']['name']))
			form_set_error('reference_image', t('Please upload the abstract file.'));*/
		/* check for valid filename extensions */
		foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
		{
			if ($file_name)
			{
				/* checking file type */
				$allowed_extensions_str = variable_get('reference_image_extensions', '');
				$allowed_extensions = explode(',', $allowed_extensions_str);
				$fnames = explode('.', strtolower($_FILES['files']['name'][$file_form_name]));
				$temp_extension = end($fnames);
				if (!in_array($temp_extension, $allowed_extensions))
					form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
				if ($_FILES['files']['size'][$file_form_name] <= 0)
					form_set_error($file_form_name, t('File size cannot be zero.'));
				/* check if valid file name */
				if (!arduino_projects_blog_check_valid_filename($_FILES['files']['name'][$file_form_name]))
					form_set_error($file_form_name, t('Invalid file name specified. Only alphabets and numbers are allowed as a valid filename.'));
			} //$file_name
		} //$_FILES['files']['name'] as $file_form_name => $file_name
	} 
	return $form_state;
}
function arduino_projects_blog_proposal_form_submit($form, &$form_state)
{
	global $user;
	$root_path = arduino_projects_blog_files_path();
	if (!$user->uid)
	{
		drupal_set_message('It is mandatory to login on this website to access the proposal form', 'error');
		return;
	} //!$user->uid
	/* inserting the user proposal */
	$v = $form_state["values"];
	$project_title = trim($v['project_title']);
	$proposar_name = $v['contributor_name'];
	$directory_name = _apb_dir_name($project_title, $proposar_name);
	list($d, $m, $y) = explode('-', $form_state['values']['expected_date_of_completion']);
	$expected_completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);
	$result = "INSERT INTO {arduino_projects_blog_proposal} 
    (
    uid, 
    approver_uid,
    contributor_name,
    contact_no,
    university,
    city, 
    pincode, 
    state, 
    country,
    position,
    how_did_you_hear_about_us,
    project_title, 
    abstract,
    reference,
    directory_name,
    approval_status,
    is_completed, 
    dissapproval_reason,
    creation_date,
    expected_date_of_completion,
    approval_date    
    ) VALUES
    (
    :uid, 
    :approver_uid, 
    :contributor_name, 
    :contact_no,
    :university, 
    :city, 
    :pincode, 
    :state,  
    :country,
    :position,
    :how_did_you_hear_about_us,
    :project_title, 
    :abstract,
    :reference,
    :directory_name,
    :approval_status,
    :is_completed, 
    :dissapproval_reason,
    :creation_date,
    :expected_date_of_completion,
    :approval_date
    )";
	$args = array(
		":uid" => $user->uid,
		":approver_uid" => 0,
		":contributor_name" => trim($v['contributor_name']),
		":contact_no" => $v['contributor_contact_no'],
		":university" => $v['university'],
		":city" => $v['city'],
		":pincode" => $v['pincode'],
		":state" => $v['all_state'],
		":country" => $v['country'],
		":position" => $v['position'],
		":how_did_you_hear_about_us" => $v['how_did_you_hear_about_us'],
		":project_title" => $v['project_title'],
		":abstract" => $v['abstract'],
		":reference" => $v['reference'],
		":directory_name" => $directory_name,
		":approval_status" => 0,
		":is_completed" => 0,
		":dissapproval_reason" => "NULL",
		":creation_date" => time(),
		":expected_date_of_completion" => $expected_completion_date_timestamp,
		":approval_date" => 0
		);
	//	var_dump($args);die;
	//var_dump($result);die;
	$proposal_id = db_query($result, $args, array(
		'return' => Database::RETURN_INSERT_ID
	));
	$proposal_dir_path = $directory_name . '/';
	$dest_path = $proposal_dir_path . 'proposal_images/';
	//var_dump($dest_path1);die;	
	if (!is_dir($root_path . $proposal_dir_path))
			mkdir($root_path . $proposal_dir_path);
	//var_dump($root_path . $proposal_dir_path);die;
	if (!is_dir($root_path . $dest_path))
			mkdir($root_path . $dest_path);
	//var_dump($root_path . $dest_path);die;
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
                if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name])) {
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
        if (!$result && $imageupload == 0) {
            drupal_set_message(t("Something went wrong, please try again."), "error");
        } 
        else if ($imageupload != 0) {
                drupal_set_message(t("Reference Images are added successfully"), "status");
            }
    if (!$proposal_id)
	{
		drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
		return;
	} //!$proposal_id
    /* sending email */
	$email_to = $user->mail;
	$form = variable_get('arduino_projects_blog_from_email', '');
	$bcc = variable_get('arduino_projects_blog_bcc_emails', '');
	$cc = variable_get('arduino_projects_blog_cc_emails', '');
	$params['arduino_projects_blog_proposal_received']['proposal_id'] = $proposal_id;
	$params['arduino_projects_blog_proposal_received']['user_id'] = $user->uid;
	$params['arduino_projects_blog_proposal_received']['headers'] = array(
		'From' => $form,
		'MIME-Version' => '1.0',
		'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
		'Content-Transfer-Encoding' => '8Bit',
		'X-Mailer' => 'Drupal',
		'Cc' => $cc,
		'Bcc' => $bcc
	);
	if (!drupal_mail('arduino_projects_blog', 'arduino_projects_blog_proposal_received', $email_to, user_preferred_language($user), $params, $form, TRUE))
		drupal_set_message('Error sending email message.', 'error');
	drupal_set_message(t('We have received your proposal. We will get back to you soon.'), 'status');
	drupal_goto('arduino-projects/my-proposals');
	
}

function arduino_projects_blog_my_proposal()
{
	global $user;
	/* get pending proposals to be approved */
	if ($user->uid == 0)
	{
		$msg = drupal_set_message(t('It is mandatory to login on this website to view your proposals. If you are new user please create a new account first.'), 'error');
		//drupal_goto('/pssp');
		drupal_goto('user/login', array('query' => drupal_get_destination()));
		return $msg;
	}
	$my_proposal_rows = array();
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	//$query->condition('approval_status', 2);
	$query->condition('uid',$user->uid);
	$query->orderBy('id', 'DESC');
	$my_proposals_q = $query->execute();
	while ($my_proposals_data = $my_proposals_q->fetchObject())
	{
		if($my_proposals_data->approval_status == 2){
			$action = l('Edit', 'arduino-projects/my-proposals/edit/' . $my_proposals_data->id);
			}
		else if($my_proposals_data->approval_status == 1 or $my_proposals_data->approval_status == 3){
			$action =	l('View', 'arduino-projects/my-proposals/view/' . $my_proposals_data->id);
			}
		else if($my_proposals_data->approval_status == 0){
			$action = 'Proposal pending for review';
		}
		$my_proposal_rows[$my_proposals_data->id] = array(
			date('d-m-Y', $my_proposals_data->creation_date),
			l($my_proposals_data->contributor_name, 'user/' . $my_proposals_data->uid),
			$my_proposals_data->project_title,
			$action
		);
	} //$pending_data = $pending_q->fetchObject()
	/* check if there are any pending proposals */
	if (!$my_proposal_rows)
	{
		drupal_set_message(t('You do not have any active submissions. Click here to propose'), 'status');
		return '';
	} //!$pending_rows
	$my_proposal_header = array(
		'Date of Submission',
		'Contributor Name',
		'Title of the Project',
		'Action'
	);
	//$output = theme_table($pending_header, $pending_rows);
	$output = theme('table', array(
		'header' => $my_proposal_header,
		'rows' => $my_proposal_rows
	));
	return $output;
}