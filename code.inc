<?php
// $Id$
function upload_examples()
  {
    return drupal_get_form('upload_examples_form');
  }
function upload_examples_form($form, &$form_state, $no_js_use = FALSE)
  {
    global $user;
    /************************ start approve book details ************************/
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message("Please submit a " . l('proposal', 'arduino-projects/proposal') . ".", 'error');
        drupal_goto('');
      }
    if ($proposal_data->approval_status != 1)
      {
        switch ($proposal_data->approval_status)
        {
            case 0:
                drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
                drupal_goto('arduino-projects');
                return;
                break;
            case 2:
                drupal_set_message(t('Your proposal has been marked for resubmission. Please update your proposal' . l('here', 'arduino-projects/my-proposals') . 'and submit.'), 'status');
                drupal_goto('arduino-projects/my-proposals');
                return;
                break;
            case 3:
                drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'status');
                drupal_goto('');
                return;
                break;
            case 5:
                  drupal_set_message(t('You have submitted your all codes.'), 'status');
                  drupal_goto('arduino-projects');
                  return;
            default:
                drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
                drupal_goto('arduino-projects');
                return;
                break;
        }
      }
    /*$preference_q = db_query("SELECT * FROM {arduino_projects_blog_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $proposal_data = db_fetch_object($preference_q);*/

    if (!$proposal_data)
      {
        drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
        drupal_goto('');
        return;
      }
    $form['#attributes'] = array(
        'enctype' => "multipart/form-data"
    );
$form['proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_data->id,    
  );
    $form['project_title'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->project_title,
        '#title' => t('Title of the Project')
    );
    $form['contributor_name'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->contributor_name,
        '#title' => t('Contributor Name')
    );
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('proposal_id', $proposal_data->id);
    //$query->condition('approval_status', 1);
    //$query->range(0, 1);
    $step_numbers = array();
    $result = $query->execute();
    //$i = 1;
    while ($step_data = $result->fetchObject()){
        $step_numbers[$step_data->number] = $step_data->number;
    }
    $options = array(
        '' => '(Select)'
    );
    for ($i = 1; $i <= 100; $i++)
      {
        $options[$i] = $i;
      }
      $step_options = array_diff($options, $step_numbers);
    $form['number'] = array(
    '#type' => 'select',
    '#title' => t('Step No'),
    '#options' => $step_options,   
    '#multiple' => FALSE,
    '#size' => 1,
    '#required' => TRUE,  
    '#ajax' => array(
        'callback' => 'ajax_step_name_callback',
        ),
    );
    $number = isset($form_state['values']['number']) ? $form_state['values']['number'] : key($options);
    $query = db_select('arduino_projects_blog_step_titles');
    $query->fields('arduino_projects_blog_step_titles');
    $query->condition('step_no', $number);
    $result = $query->execute();
    $step_data = $result->fetchObject();
    $form['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Title of the Step'),
        '#size' => 40,
        '#maxlength' => 255,
        '#required' => TRUE,    
        '#prefix' => '<div id="ajax-step-name-replace">',
        '#suffix' => '</div>',    
    );
    $form['description'] = array(
        '#type' => 'text_format',
        '#title' => t('Description'),
        '#required' => TRUE
    );
    $form['step_images'] = array(
            '#type' => 'fieldset',
            '#tree' => TRUE,
            '#prefix' => '<div id="names-fieldset-wrapper">',
            '#suffix' => '</div>',
            '#title' => t('Upload images related to the step'),
        );
        if (empty($form_state['image_names'])) {
            $form_state['image_names'] = 1;
        }
        for ($i = 0; $i < $form_state['image_names']; $i++) {
            $temp_no = $i;
            $form['step_images'][$i]['image'] = array(
                //'#title' => t('Add Event Image'),
                '#type' => 'file',
                '#weight' => '5',
                '#description' => t('Upload filenames with allowed extensions only. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions: ') . variable_get('step_image_extensions', '') . t('</span>'),
                // We need this to know which file element this is.
                // By default drupal would name all as files[step_images]
                '#name' => 'files[step_images_' . $i . '_name]',
                //'#attributes' => array('multiple' => 'multiple'),
                //'#upload_location' => 'public://',
                '#upload_validators' => array(
                    // Pass the maximum file size in bytes
                    'file_validate_size' => array(2*1024),
                  ),
            );
            $form['step_images']["images_count"] = array(
                "#type" => "hidden",
                "#value" => $temp_no
            );
        }
        if($i < 5){
        $form['step_images']['add_image'] = array(
            '#type' => 'submit',
            '#value' => t('Add Image'),
            '#limit_validation_errors' => array(),
            '#submit' => array(
                'step_images_add_more_add_one'
            ),
            '#ajax' => array(
                'callback' => 'step_images_add_more_callback',
                'wrapper' => 'names-fieldset-wrapper'
            )
        );
    }
        if ($form_state['image_names'] > 1) {
            $form['step_images']['remove_image'] = array(
                '#type' => 'submit',
                '#value' => t('Remove Image'),
                '#limit_validation_errors' => array(),
                '#submit' => array(
                    'step_images_add_more_remove_one'
                ),
                '#ajax' => array(
                    'callback' => 'step_images_add_more_callback',
                    'wrapper' => 'names-fieldset-wrapper'
                )
            );
        }
        if ($no_js_use) {
            if (!empty($form['step_images']['remove_image']['#ajax'])) {
                unset($form['step_images']['remove_image']['#ajax']);
            }
            unset($form['step_images']['add_image']['#ajax']);
        }
    $form['step_video'] = array(
        '#type' => 'textarea',
        '#title' => t('Upload Video related to the step'),
        '#size' => 600,
        '#maxlength' => 600,
    );
    $form['step_gifs'] = array(
            '#type' => 'fieldset',
            '#tree' => TRUE,
            '#prefix' => '<div id="gifs-fieldset-wrapper">',
            '#suffix' => '</div>',
            '#title' => t('Upload gifs related to the step'),
        );
        if (empty($form_state['gif_names'])) {
            $form_state['gif_names'] = 1;
        }
        for ($p = 0; $p < $form_state['gif_names']; $p++) {
            $temp_no = $p;
            $form['step_gifs'][$p]['gifs'] = array(
                //'#title' => t('Add Event Image'),
                '#type' => 'file',
                '#weight' => '5',
                '#description' => t('Upload filenames with allowed extensions only. No spaces or any special characters allowed in filename.') . '<br />' . t('<span style="color:red;">Allowed file extensions: gif</span>'),
                // We need this to know which file element this is.
                // By default drupal would name all as files[step_gifs]
                '#name' => 'files[step_gifs_' . $p . '_name]',
                //'#attributes' => array('multiple' => 'multiple'),
                //'#upload_location' => 'public://',
                '#upload_validators' => array(
                    'file_validate_extensions' => ['gif'],
                    // Pass the maximum file size in bytes
                    'file_validate_size' => array(2*1024),
                  ),
            );
            $form['step_gifs']["images_count"] = array(
                "#type" => "hidden",
                "#value" => $temp_no
            );
        }
        if($p < 5){
        $form['step_gifs']['add_gif'] = array(
            '#type' => 'submit',
            '#value' => t('Add Gif'),
            '#limit_validation_errors' => array(),
            '#submit' => array(
                'step_gifs_add_more_add_one'
            ),
            '#ajax' => array(
                'callback' => 'step_gifs_add_more_callback',
                'wrapper' => 'gifs-fieldset-wrapper'
            )
        );
    }
        if ($form_state['gif_names'] > 1) {
            $form['step_gifs']['remove_gif'] = array(
                '#type' => 'submit',
                '#value' => t('Remove Gif'),
                '#limit_validation_errors' => array(),
                '#submit' => array(
                    'step_gifs_add_more_remove_one'
                ),
                '#ajax' => array(
                    'callback' => 'step_gifs_add_more_callback',
                    'wrapper' => 'gifs-fieldset-wrapper'
                )
            );
        }
        if ($no_js_use) {
            if (!empty($form['step_gifs']['remove_gif']['#ajax'])) {
                unset($form['step_gifs']['remove_gif']['#ajax']);
            }
            unset($form['step_gifs']['add_gif']['#ajax']);
        }
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    $form['cancel'] = array(
        '#type' => 'markup',
        '#value' => l(t('Cancel'), 'arduino-projects/code')
    );
    return $form;
  }
function step_images_add_more_callback($form, $form_state) {
    return $form['step_images'];
}


function step_images_add_more_add_one($form, &$form_state) {
    $form_state['image_names']++;
    $form_state['rebuild'] = TRUE;
    //$form_state['no_redirect'] = TRUE;
}


function step_images_add_more_remove_one($form, &$form_state) {
    if ($form_state['image_names'] > 1) {
        $form_state['image_names']--;
    }
    $form_state['rebuild'] = TRUE;
}
function step_gifs_add_more_callback($form, $form_state) {
    return $form['step_gifs'];
}


function step_gifs_add_more_add_one($form, &$form_state) {
    $form_state['gif_names']++;
    $form_state['rebuild'] = TRUE;
    //$form_state['no_redirect'] = TRUE;
}


function step_gifs_add_more_remove_one($form, &$form_state) {
    if ($form_state['gif_names'] > 1) {
        $form_state['gif_names']--;
    }
    $form_state['rebuild'] = TRUE;
}

function upload_examples_form_validate($form, &$form_state)
  {
    /*if (!arduino_projects_blog_check_name($form_state['values']['name']))
        form_set_error('name', t('Title of the Step can contain only alphabets, numbers and spaces.'));*/
  /*  if (isset($_FILES['files']))
      {

        /* check if atleast one source or result file is uploaded */
        //if ($_FILES['files']['name']['sourcefile1'] || $_FILES['files']['name']['upload_dataset'])
           // form_set_error('sourcefile1', t('Please upload source file.'));
        /* check for valid filename extensions 
        foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
          {
            if ($file_name)
              {
                if (strstr($file_form_name, 'image'))
                        $file_type = 'I';
                else if (strstr($file_form_name, 'gifs'))
                        $file_type = 'G';
                $allowed_extensions_str = '';
                switch ($file_type)
                    {
                        case 'I':
                            $allowed_extensions_str = variable_get('step_image_extensions', '');
                            break;
                        case 'G':
                            $allowed_extensions_str = 'gif';
                            break;
                    }
                //$allowed_extensions_str = variable_get('arduino_projects_blog_source_extensions', '');
                $allowed_extensions = explode(',', $allowed_extensions_str);
                $temp_ext = explode('.', strtolower($_FILES['files']['name'][$file_form_name]));
                $temp_extension = end($temp_ext);
                $temp_extension = substr($_FILES['files']['name'][$file_form_name], strripos($_FILES['files']['name'][$file_form_name], '.')); // get file name
                //var_dump($temp_extension); die;
                if (!in_array($temp_extension, $allowed_extensions))
                    form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
                if ($_FILES['files']['size'][$file_form_name] <= 0)
                    form_set_error($file_form_name, t('File size cannot be zero.'));
                /* check if valid file name 
                if (!arduino_projects_blog_check_valid_filename($_FILES['files']['name'][$file_form_name]))
                form_set_error($file_form_name, t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));
              }
          }
      }*/
  }
function upload_examples_form_submit($form, &$form_state)
  {
    global $user;
    $root_path = arduino_projects_blog_files_path();
    /************************ start approve book details ************************/
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message("Please submit a " . l('proposal', 'arduino-projects/proposal') . ".", 'error');
        drupal_goto('');
      }
    if ($proposal_data->approval_status != 1 && $proposal_data->approval_status != 4)
      {
        switch ($proposal_data->approval_status)
        {
            case 0:
                drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
                drupal_goto('');
                return;
                break;
            case 2:
                drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'error');
                drupal_goto('');
                return;
                break;
            case 3:
                drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'status');
                drupal_goto('');
                return;
                break;
case 5:
      drupal_set_message(t('You have submmited your all codes'), 'status');
      drupal_goto('');
      return;
      break;
            default:
                drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
                drupal_goto('');
                return;
                break;
        }
      }
    /*$preference_q = db_query("SELECT * FROM {arduino_projects_blog_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $proposal_data = db_fetch_object($preference_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('id', $proposal_data->id);
    $query->condition('approval_status', 1);
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
        drupal_goto('');
        return;
      }
    /************************ end approve book details **************************/
    $proposal_directory = $proposal_data->directory_name;
    $dest_path = $proposal_directory . '/';
    if (!is_dir($root_path . $dest_path)){   
        if(!mkdir($root_path . $dest_path))
        {
        drupal_set_message(t('You cannot upload your code. Error in creating directory'), 'error');
        }
     }   
    /* inserting step details */
    $step_id = 0;
    /*$step_result = db_query("SELECT * FROM {arduino_projects_blog_step} WHERE preference_id = %d AND number = %d", $preference_id, $form_state['values']['number']);*/

    //var_dump($form_state['values']['name']);die;
    $proposal_id = $proposal_data->id;
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('proposal_id', $proposal_id);
    $query->condition('number', $form_state['values']['number']);
    $step_result = $query->execute();
    if (!$step_row = $step_result->fetchObject())
      {
        $query = "INSERT INTO {arduino_projects_blog_step} (proposal_id, number, name) VALUES(:proposal_id, :number, :name)";
        $args = array(
            ":proposal_id" => $proposal_id,
            ":number" => $form_state['values']['number'],
            ":name" => $form_state['values']['name']
        );
        $result = db_query($query, $args, array(
            'return' => Database::RETURN_INSERT_ID
        ));
        $step_id = $result;
      }
    else
      {
        $step_id = $step_row->id;
        $query = db_update('arduino_projects_blog_step');
        $query->fields(array(
            'name' => $form_state['values']['name']
        ));
        $query->condition('id', $step_id);
        $num_updated = $query->execute();
      }
    $query = db_select('arduino_projects_blog_step_information');
    $query->fields('arduino_projects_blog_step_information');
    $query->condition('step_id', $step_id);
    $cur_example_q = $query->execute();
    if ($cur_example_d = $cur_example_q->fetchObject())
      {
        if ($cur_example_d->approval_status == 1)
          {
            drupal_set_message(t("Example already approved. Cannot overwrite it."), 'error');
            drupal_goto('arduino-projects/code');
            return;
          }
        else if ($cur_example_d->approval_status == 0)
          {
            drupal_set_message(t("Example is under pending review. Delete the example and reupload it."), 'error');
            drupal_goto('arduino-projects/code');
            return;
          }
        else
          {
            drupal_set_message(t("Error uploading example. Please contact administrator."), 'error');
            drupal_goto('arduino-projects/code');
            return;
          }
      }
    /* creating directories */
    $dest_path .= 'project_files/';
    if (!is_dir($root_path . $dest_path))
        mkdir($root_path . $dest_path);
    $dest_path .= 'Step' . $form_state['values']['number'] . '/';
    if (!is_dir($root_path . $dest_path))
        mkdir($root_path . $dest_path);
    $filepath = $proposal_directory . '/project_files/Step' . $form_state['values']['number'] . '/';
    $query = "INSERT INTO {arduino_projects_blog_step_information} (step_id, description, description_type, video, approval_date, approval_status, timestamp) VALUES (:step_id, :description, :description_type, :video, :approval_date,:approval_status, :timestamp)";
    $args = array(
        ":step_id" => $step_id,
        ":description" => $form_state['values']['description']['value'],
        ":description_type" => $form_state['values']['description']['format'],
        ":video" => $form_state['values']['step_video'],
        ":approval_date" => time(),
        ":approval_status" => 0,
        ":timestamp" => time()
    );
    $result = db_query($query, $args, array(
        'return' => Database::RETURN_INSERT_ID
    ));
    //$step_id = $result->step_id;
    
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
      {
        if ($file_name)
          {
            /* checking file type */
            if (strstr($file_form_name, 'images'))
            {
                $file_type = 'I';
            } //strstr($file_form_name, 'upload_flowsheet_developed_process')
            else if (strstr($file_form_name, 'gifs'))
            {
                $file_type = 'G';
            }

            //$file_type = 'S';
            switch ($file_type) {
                case 'I':
                    $dest_path = $filepath . 'images/';
                    //var_dump($root_path . $dest_path);die;
                    if (!is_dir($root_path . $dest_path))
                        mkdir($root_path . $dest_path);
                    if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
                      {
                        drupal_set_message(t("Error uploading file. File !filename already exists.", array(
                            '!filename' => $_FILES['files']['name'][$file_form_name]
                        )), 'error');
                        return;
                      }
                    /* uploading file */
                    else if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
                      {
                        $query = "INSERT INTO {arduino_projects_blog_step_files} (step_id, filename, filepath,filemime, filesize, filetype, timestamp)
                  VALUES (:step_id, :filename ,:filepath,:filemime, :filesize, :filetype, :timestamp)";
                        $args = array(
                            ":step_id" => $step_id,
                            ":filename" => $_FILES['files']['name'][$file_form_name],
                            ":filepath" => $dest_path . $_FILES['files']['name'][$file_form_name],
                            ":filemime" => mime_content_type($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]),
                            ":filesize" => $_FILES['files']['size'][$file_form_name],
                            ":filetype" => $file_type,
                            ":timestamp" => time()
                        );
                        $result = db_query($query, $args, array(
                            'return' => Database::RETURN_INSERT_ID
                        ));
                        drupal_set_message($file_name . ' uploaded successfully.', 'status');
                      }
                    else
                      {
                        drupal_set_message('Error uploading file : ' . $dest_path . $file_name, 'error');
                      }
                      break;
                case 'G':
                    $dest_path = $filepath . 'gifs/';
                    //var_dump($root_path . $dest_path);die;
                    if (!is_dir($root_path . $dest_path))
                        mkdir($root_path . $dest_path);
                    //var_dump($root_path . $dest_path);die;
                    if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
                      {
                        drupal_set_message(t("Error uploading file. File !filename already exists.", array(
                            '!filename' => $_FILES['files']['name'][$file_form_name]
                        )), 'error');
                        return;
                      }
                    /* uploading file */
                    else if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
                      {
                        $query = "INSERT INTO {arduino_projects_blog_step_files} (step_id, filename, filepath,filemime, filesize, filetype, timestamp)
                  VALUES (:step_id, :filename ,:filepath,:filemime, :filesize, :filetype, :timestamp)";
                        $args = array(
                            ":step_id" => $step_id,
                            ":filename" => $_FILES['files']['name'][$file_form_name],
                            ":filepath" => $dest_path . $_FILES['files']['name'][$file_form_name],
                            ":filemime" => mime_content_type($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]),
                            ":filesize" => $_FILES['files']['size'][$file_form_name],
                            ":filetype" => $file_type,
                            ":timestamp" => time()
                        );
                        $result = db_query($query, $args, array(
                            'return' => Database::RETURN_INSERT_ID
                        ));
                        drupal_set_message($file_name . ' uploaded successfully.', 'status');
                      }
                    else
                      {
                        drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
                      }
                      break;
            }
          }
      }
  
    drupal_set_message('Step uploaded successfully.', 'status');
	/* sending email */
	/*$email_to = $user->mail;
	$from = variable_get('arduino_projects_blog_from_email', '');
	$bcc = variable_get('arduino_projects_blog_emails', '');
	$cc = variable_get('arduino_projects_blog_cc_emails', '');
	$params['example_uploaded']['example_id'] = $example_id;
	$params['example_uploaded']['user_id'] = $user->uid;
	$params['example_uploaded']['headers'] = array(
		'From' => $from,
		'MIME-Version' => '1.0',
		'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
		'Content-Transfer-Encoding' => '8Bit',
		'X-Mailer' => 'Drupal',
		'Cc' => $cc,
		'Bcc' => $bcc
	);
	if (!drupal_mail('arduino_projects_blog', 'example_uploaded', $email_to, language_default(), $params, $from, TRUE))
		drupal_set_message('Error sending email message.', 'error');*/
	drupal_goto('arduino-projects/code');
    
    
  }
/******************************************************************************/
/***************************** DELETE EXAMPLE *********************************/
/******************************************************************************/
function _upload_examples_delete()
  {
    global $user;
    $root_path = arduino_projects_blog_path();
    $example_id = arg(3);
    //var_dump($example_id);die;
    /* check example */
    /*$example_q = db_query("SELECT * FROM {arduino_projects_blog_step_information} WHERE id = %d LIMIT 1", $example_id);
    $example_data = db_fetch_object($example_q);*/
    $query = db_select('arduino_projects_blog_step_information');
    $query->fields('arduino_projects_blog_step_information');
    $query->condition('id', $example_id);
    $query->range(0, 1);
    $result = $query->execute();
    $example_data = $result->fetchObject();
    if (!$example_data)
      {
        drupal_set_message('Invalid example.', 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    if ($example_data->approval_status != 0)
      {
        drupal_set_message('You cannnot delete an example after it has been approved. Please contact site administrator if you want to delete this example.', 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    /*$step_q = db_query("SELECT * FROM {arduino_projects_blog_step} WHERE id = %d LIMIT 1", $example_data->step_id);
    $step_data = db_fetch_object($step_q);*/
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('id', $example_data->step_id);
    $query->range(0, 1);
    $result = $query->execute();
    $step_data = $result->fetchObject();
    if (!$step_data)
      {
        drupal_set_message('You do not have permission to delete this example.', 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    /*$preference_q = db_query("SELECT * FROM {arduino_projects_blog_preference} WHERE id = %d LIMIT 1", $step_data->preference_id);
    $proposal_data = db_fetch_object($preference_q);*/
    $query = db_select('arduino_projects_blog_preference');
    $query->fields('arduino_projects_blog_preference');
    $query->condition('id', $step_data->preference_id);
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message('You do not have permission to delete this example.', 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE id = %d AND uid = %d LIMIT 1", $proposal_data->proposal_id, $user->uid);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('id', $proposal_data->proposal_id);
    $query->condition('uid', $user->uid);
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message('You do not have permission to delete this example.', 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    /* deleting example files */
    if (delete_example($example_data->id))
      {
        drupal_set_message('Example deleted.', 'status');
        /* sending email */
      
        
        $email_to = $user->mail;
		$from = variable_get('arduino_projects_blog_from_email', '');
		$bcc = variable_get('arduino_projects_blog_emails', '');
		$cc = variable_get('arduino_projects_blog_cc_emails', '');
		$params['example_deleted_user']['book_title'] = $proposal_data->book;
        $params['example_deleted_user']['step_title'] = $step_data->name;
        $params['example_deleted_user']['example_number'] = $example_data->number;
        $params['example_deleted_user']['example_caption'] = $example_data->caption;
        $params['example_deleted_user']['user_id'] = $user->uid;
		$params['example_deleted_user']['headers'] = array(
		'From' => $from,
		'MIME-Version' => '1.0',
		'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
		'Content-Transfer-Encoding' => '8Bit',
		'X-Mailer' => 'Drupal',
		'Cc' => $cc,
		'Bcc' => $bcc
	);
        if (!drupal_mail('arduino_projects_blog', 'example_deleted_user', $email_to, language_default(), $params, $from, TRUE))
            drupal_set_message('Error sending email message.', 'error');
      }
    else
      {
        drupal_set_message('Error deleting example.', 'status');
      }
    drupal_goto('arduino-projects/code');
    return;
  }
/******************************************************************************/
/************************** GENERAL FUNCTIONS *********************************/
/******************************************************************************/
function _list_of_book_titles()
  {
    $book_titles = array(
        '0' => 'Please select...'
    );
    /*$book_titles_q = db_query("SELECT * FROM {arduino_projects_blog_preference} WHERE approval_status = 1 OR approval_status = 3 ORDER BY book ASC");*/
    $query = db_select('arduino_projects_blog_preference');
    $query->fields('arduino_projects_blog_preference');
    $or = db_or();
    $or->condition('approval_status', 1);
    $or->condition('approval_status', 3);
    $query->condition($or);
    $query->orderBy('book', 'ASC');
    $book_titles_q = $query->execute();
    while ($book_titles_data = $book_titles_q->fetchObject())
      {
        $book_titles[$book_titles_data->id] = $book_titles_data->book . ' (Written by ' . $book_titles_data->author . ')';
      }
    return $book_titles;
  }
function _list_of_book_dependency_files()
  {
    $book_dependency_files = array();
    $book_dependency_files_class = array();
    /*$book_dependency_files_q = db_query("SELECT * FROM {arduino_projects_blog_dependency_files} ORDER BY filename ASC");*/
    $query = db_select('arduino_projects_blog_dependency_files');
    $query->fields('arduino_projects_blog_dependency_files');
    $query->orderBy('filename', 'ASC');
    $book_dependency_files_q = $query->execute();
    while ($book_dependency_files_data = $book_dependency_files_q->fetchObject())
      {
        $temp_caption = '';
        if ($book_dependency_files_data->caption)
            $temp_caption .= ' (' . $book_dependency_files_data->caption . ')';
        $book_dependency_files[$book_dependency_files_data->id] = l($book_dependency_files_data->filename . $temp_caption, 'download/dependency/' . $book_dependency_files_data->id, array(
            'attributes' => array(
                'class' => $book_dependency_files_data->preference_id
            )
        ));
        $book_dependency_files_class[$book_dependency_files_data->id] = $book_dependency_files_data->preference_id;
      }
    return array(
        $book_dependency_files,
        $book_dependency_files_class
    );
  }

function ajax_step_name_callback($form, $form_state){

  $step_number = $form_state['values']['number'];
  if($step_number <= 5){
  $query = "SELECT * FROM `arduino_projects_blog_step_titles` WHERE `step_no` = :number";
      $arg = array(':number' => $step_number);
      $step_name_data = db_query($query, $arg);
      
      if ($step_name_data->rowCount() > 0) { 

      //$form['name']['#default_value'] = $step_name_data->fetchObject()->name; 
      $form['name']['#value'] = $step_name_data->fetchObject()->step_title;    
      $form['name']['#readonly'] = TRUE;
      $form['name']['#attributes']['readonly'] = 'readonly';
      $commands[] = ajax_command_replace("#ajax-step-name-replace", drupal_render($form['name']));      
      
      }
  }else{
      $form['name']['#value'] = ' ';
      $form['name']['#readonly'] = FALSE;
      //$form['name']['#attributes']['readonly'] = 'readonly';
      $commands[] = ajax_command_replace("#ajax-step-name-replace", drupal_render($form['name']));
    
    }
      return array('#type' => 'ajax', '#commands' => $commands);

}