<?php
function introduction_approval()
{
    /* get a list of unapproved introductions */
    $rows = array();
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('approval_status', 1);
    $query->condition('submitted_steps', 1);
    $proposal_q = $query->execute();
    while($proposal_data = $proposal_q->fetchObject()){
    $intro_query = db_select('arduino_projects_blog_introduction_step');
    $intro_query->fields('arduino_projects_blog_introduction_step');
    $intro_query->condition('proposal_id', $proposal_data->id);
    $intro_query->condition('is_submitted', 1);
    $intro_query->condition('approval_status', 0);
    $intro_query_result  = $intro_query->execute();
    $intro_query_data = $intro_query_result->fetchObject();
         $rows[] = array(
            $proposal_data->project_title,
            $proposal_data->contributor_name,
            l('Edit', 'arduino-projects/code-approval/introduction/' . $intro_query_data->id)
        );
    }
    
    /* check if there are any pending proposals */
    if (!$rows) {
        drupal_set_message(t('There are no pending proposals'), 'status');
        return '';
    }
    $header = array(
        'Title of the Book',
        'Contributor Name',
        'Actions'
    );
    $output = theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
    return $output;
}

function introduction_approval_form($form, &$form_state)
{
    /* get a list of unapproved chapters */
    $id = arg(3);
    
      /*  $query = db_select('arduino_projects_blog_proposal');
        $query->fields('arduino_projects_blog_proposal');
        $query->condition('id', $id);
        $result = $query->execute();
        $preference_data = $result->fetchObject();*/
        //var_dump($preference_data);die;
        /* get proposal data */
        /*$proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $preference_data->proposal_id);
        $proposal_data = db_fetch_object($proposal_q);*/
        $query = db_select('arduino_projects_blog_introduction_step');
        $query->fields('arduino_projects_blog_introduction_step');
        $query->condition('id', $id);
        $result = $query->execute();
        $intro_data = $result->fetchObject();
        $query_proposal = db_select('arduino_projects_blog_proposal');
        $query_proposal->fields('arduino_projects_blog_proposal');
        $query_proposal->condition('id', $intro_data->proposal_id);
        $proposal_data = $query_proposal->execute()->fetchObject();
    $form['#tree'] = TRUE;
    $form['contributor'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->contributor_name,
        '#title' => t('Contributor Name')
    );
    $form['book_details']['book'] = array(
        '#type' => 'item',
        '#markup' => $proposal_data->project_title,
        '#title' => t('Title of the Book')
    );
    
    /* get example data */
    /*$example_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d AND approval_status = 0", $chapter_id);*/
    
        /*$form['example_details'][$example_data->id] = array(
            '#type' => 'fieldset',
            '#collapsible' => FALSE,
            '#collapsed' => TRUE
        );*/
        $form['description'] = array(
            '#type' => 'item',
            '#markup' => $intro_data->description,
            '#title' => t('Description')
        );
        $form['video'] =array(
            '#type' => 'item',
            '#markup' => convertLinkToEmbed($intro_data->demo_video, 450, 250),
            '#title' => t('Demo Video')
        );
        /*$form['download_file'] = array(
            '#type' => 'item',
            '#title' => t('Click to download the file with list of codable examples'),
            '#markup' => l('Download Example', 'arduino-projects/download/codable-example/' . $example_data->proposal_id)
        );*/
        $form['intro_status'] = array(
            '#type' => 'radios',
            '#options' => array(
                'Approved',
                'Dis-approved'
            )
        );
        $form['message'] = array(
            '#type' => 'textarea',
            '#size' => 2500,
            '#maxlength' => 2500,
            '#title' => t('Reason for dis-approval')
        );
        $form['proposal_id'] = array(
            '#type' => 'hidden',
            '#value' => $proposal_data->id
        );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    $form['back_to_list'] = array(
        '#type' => 'item',
        '#markup' => l('Back to Code Approval List', 'arduino-projects/code-approval')
    );
    return $form;
}

function introduction_approval_form_submit($form, &$form_state){
    global $user;
    $proposal_id = $form_state['values']['proposal_id'];
    $query = db_select('arduino_projects_blog_introduction_step');
        $query->fields('arduino_projects_blog_introduction_step');
        $query->condition('proposal_id', $proposal_id);
        $result = $query->execute();
        $intro_data = $result->fetchObject();
     $query = db_select('arduino_projects_blog_proposal');
        $query->fields('arduino_projects_blog_proposal');
        $query->condition('id', $intro_data->proposal_id);
        $result = $query->execute();
        $proposal_data = $result->fetchObject();
    $user_data = user_load($proposal_data->uid);
    $ex_data = $form_state['values'];
    if ($ex_data['intro_status'] == "0") {
            /*db_query("UPDATE {textbook_companion_example} SET approval_status = 1, approver_uid = %d, approval_date = %d WHERE id = %d", $user->uid, time(), $ex_data['example_id']);*/
            $query = db_update('arduino_projects_blog_introduction_step');
            $query->fields(array(
                'approval_status' => 1,
            ));
            $query->condition('proposal_id', $ex_data['proposal_id']);
            $num_updated = $query->execute();
            /* sending email */
            $email_to = $user_data->mail;
            $from = variable_get('textbook_companion_from_email', '');
            $bcc = variable_get('textbook_companion_emails', '');
            $cc = variable_get('textbook_companion_cc_emails', '');
            $param['codable_example_approved']['proposal_id'] = $ex_data['proposal_id'];
            $param['codable_example_approved']['user_id'] = $user_data->uid;
            $param['codable_example_approved']['headers'] = array(
                'From' => $from,
                'MIME-Version' => '1.0',
                'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
                'Content-Transfer-Encoding' => '8Bit',
                'X-Mailer' => 'Drupal',
                'Cc' => $cc,
                'Bcc' => $bcc
            );
            if (!drupal_mail('textbook_companion', 'intro_approved', $email_to, language_default(), $param, $from, TRUE))
                drupal_set_message('Error sending email message.', 'error');
            drupal_set_message('Introduction approved.', 'status');
            drupal_goto('arduino-projects/code-approval/');
        } else if ($ex_data['intro_status'] == "1") {
            
                /* sending email */
                $query = db_update('arduino_projects_blog_introduction_step');
            $query->fields(array(
                'approval_status' => 0,
                'disapproval_message' => $ex_data['message']
            ));
            $query->condition('proposal_id', $ex_data['proposal_id']);
            $num_updated = $query->execute();
             $pro_query = db_update('arduino_projects_blog_proposal');
            $pro_query->fields(array(
                'submitted_steps' => 0
                ));
            $pro_query->condition('id', $ex_data['proposal_id']);
            $pro_updated = $pro_query->execute();
                $email_to = $user_data->mail;
                $from = variable_get('textbook_companion_from_email', '');
                $bcc = variable_get('textbook_companion_emails', '');
                $cc = variable_get('textbook_companion_cc_emails', '');
                $param['codable_example_disapproved']['proposal_id'] = $ex_data['proposal_id'];
                $param['codable_example_disapproved']['user_id'] = $user_data->uid;
                $param['codable_example_disapproved']['message'] = $ex_data['message'];
                $param['codable_example_disapproved']['headers'] = array(
                    'From' => $from,
                    'MIME-Version' => '1.0',
                    'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
                    'Content-Transfer-Encoding' => '8Bit',
                    'X-Mailer' => 'Drupal',
                    'Cc' => $cc,
                    'Bcc' => $bcc
                );
                //if (!drupal_mail('textbook_companion', 'intro_disapproved', $email_to, language_default(), $param, $from, TRUE))
                  //  drupal_set_message('Error sending email message.', 'error');
                drupal_set_message('Introduction disapproved and user has been notified.', 'status');
                drupal_goto('arduino-projects/code-approval');
            }
            else {
                drupal_set_message('Error in updating the status. Please contact administrator.', 'error');
            }
}

/********************************* BULK APPROVAL ******************************/
function step_approval_form($form, &$form_state)
{
    $options_first = _step_list_of_projects();
    $options_two = _ajax_bulk_get_step_list();
    $selected = isset($form_state['values']['project_title']) ? $form_state['values']['project_title'] : key($options_first);

    $select_two = isset($form_state['values']['step']) ? $form_state['values']['step'] : key($options_two);
    $form['project_title'] = array(
        '#type' => 'select',
        '#title' => t('Title of the Project'),
        '#options' => _step_list_of_projects(),
        '#default_value' => $selected,
        '#tree' => TRUE,
        '#ajax' => array(
            'callback' => 'ajax_bulk_step_list_callback'
        ),
        '#validated' => TRUE
    );
    /*$form['book_actions'] = array(
        '#type' => 'select',
        '#title' => t('Please select action for selected book'),
        '#options' => _bulk_list_book_actions(),
        //'#default_value' => isset($form_state['values']['lab_actions']) ? $form_state['values']['lab_actions'] : 0,
        '#prefix' => '<div id="ajax_selected_book_action" style="color:red;">',
        '#suffix' => '</div>',
        '#states' => array(
            'invisible' => array(
                ':input[name="book"]' => array(
                    'value' => 0
                )
            )
        ),
        '#validated' => TRUE
    );*/
    $form['step'] = array(
        '#type' => 'select',
        '#title' => t('Title of the Step'),
        '#options' => _ajax_bulk_get_step_list($selected),
        //'#default_value' => $chapter_default_value,
        '#prefix' => '<div id="ajax_select_step_list">',
        '#suffix' => '</div>',
        '#validated' => TRUE,
        '#tree' => TRUE,
        '#ajax' => array(
            'callback' => 'ajax_bulk_step_details_list_callback'
        ),
        '#states' => array(
            'invisible' => array(
                ':input[name="book"]' => array(
                    'value' => 0
                )
            )
        )
    );
    $form['step_details'] = array(
        '#type' => 'item',
        '#prefix' => '<div id="ajax_selected_step"></div>'
    );
    $form['step_actions'] = array(
        '#type' => 'select',
        '#title' => t('Please select action for selected step'),
        '#options' => _bulk_list_step_actions(),
        //'#default_value' => isset($form_state['values']['lab_actions']) ? $form_state['values']['lab_actions'] : 0,
        '#prefix' => '<div id="ajax_selected_step_action" style="color:red;">',
        '#suffix' => '</div>',
        '#states' => array(
            'invisible' => array(
                ':input[name="book"]' => array(
                    'value' => 0
                )
            )
        ),
        '#ajax' => array(
            'callback' => 'ajax_bulk_chapter_actions_callback'
        )
    );
    /*$form['example'] = array(
        '#type' => 'select',
        '#title' => t('Example No. (Caption)'),
        '#options' => _ajax_bulk_get_examples(),
        // '#default_value' => $example_default_value,       
        '#validated' => TRUE,
        '#prefix' => '<div id="ajax_selected_example">',
        '#suffix' => '</div>',
        '#states' => array(
            'invisible' => array(
                ':input[name="book"]' => array(
                    'value' => 0
                )
            )
        ),
        '#ajax' => array(
            'callback' => 'ajax_bulk_example_files_callback'
        )
    );*/
    /*$form['download_example'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_download_selected_example"></div>'
    );
    $form['edit_example'] = array(
        '#type' => 'item',
        '#markup' => '<div id="ajax_edit_selected_example"></div>'
    );
    $form['example_files'] = array(
        '#type' => 'item',
        '#markup' => '',
        '#prefix' => '<div id="ajax_example_files_list">',
        '#suffix' => '</div>'
    );
    $form['example_actions'] = array(
        '#type' => 'select',
        '#title' => t('Please select action for selected example'),
        '#options' => _bulk_list_example_actions(),
        //'#default_value' => isset($form_state['values']['lab_actions']) ? $form_state['values']['lab_actions'] : 0,
        '#prefix' => '<div id="ajax_selected_example_action" style="color:red;">',
        '#suffix' => '</div>',
        '#states' => array(
            'invisible' => array(
                ':input[name="book"]' => array(
                    'value' => 0
                )
            )
        )
    );*/
    $form['message'] = array(
        '#type' => 'textarea',
        '#title' => t('If Dis-Approved please specify reason for Dis-Approval'),
        '#states' => array(
            'visible' => array(
                array(
                    array(
                        ':input[name="step_actions"]' => array(
                            'value' => 2
                        )
                    ),
                    'or',
                    array(
                        ':input[name="step_actions"]' => array(
                            'value' => 3
                        )
                    ),
                )
            ),
            'required' => array(
                array(
                    array(
                        ':input[name="step_actions"]' => array(
                            'value' => 2
                        )
                    ),
                    'or',
                    array(
                        ':input[name="step_actions"]' => array(
                            'value' => 3
                        )
                    ),
                )
            )
        )
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#states' => array(
            'invisible' => array(
                ':input[name="project_title"]' => array(
                    'value' => 0
                )
            )
        )
    );
    return $form;
}
function step_approval_form_submit($form, &$form_state)
{
    global $user;
    $msg = '';
    $root_path = arduino_projects_blog_files_path();
    if ($form_state['clicked_button']['#value'] == 'Submit')
    {
        if ($form_state['values']['step'])
        // om_pssp_abstract_del_lab_pdf($form_state['values']['om_pssp_project']);
            if (user_access('arduino projects bulk manage code'))
            {
                $query = db_select('arduino_projects_blog_proposal');
                $query->fields('arduino_projects_blog_proposal');
                $query->condition('id', $form_state['values']['project_title']);
                $user_query = $query->execute();
                $user_info = $user_query->fetchObject();
                $user_data = user_load($user_info->uid);
                $steps_approved_count = $user_info->no_of_steps_approved;
                $step_no  = $form_state['values']['step'];
                if ($form_state['values']['step_actions'] == 1)
                {
                    // approving entire project //
                    $query = db_select('arduino_projects_blog_step');
                    $query->fields('arduino_projects_blog_step');
                    $query->condition('number', $form_state['values']['step']);
                    $query->condition('proposal_id', $user_info->id);
                    $result = $query->execute();
                    while($step_data = $result->fetchObject()){
                    $query = db_select('arduino_projects_blog_step_information');
                    $query->fields('arduino_projects_blog_step_information');
                    $query->condition('step_id', $step_data->id);
                    $abstracts_q = $query->execute();
                    $experiment_list = '';
                    while ($abstract_data = $abstracts_q->fetchObject())
                    {
                        //var_dump($abstract_data);die;
                        db_query("UPDATE {arduino_projects_blog_step_information} SET approval_status = 1, approver_uid = :approver_uid, approval_date = :approval_date WHERE id = :id", array(
                            ':approver_uid' => $user->uid,
                            ':approval_date' => time(),
                            ':id' => $abstract_data->id
                        ));
                        db_query("UPDATE {arduino_projects_blog_step} SET approval_status = 1, approver_uid = :approver_uid, approval_date = :approval_date WHERE id = :id", array(
                            ':approver_uid' => $user->uid,
                            ':approval_date' => time(),
                            ':id' => $abstract_data->step_id
                        ));
                        db_query("UPDATE {arduino_projects_blog_proposal} SET no_of_steps_approved = :steps_approved WHERE id = :proposal_id", array(
                            ':steps_approved' => $steps_approved_count+1,
                            ':proposal_id' => $step_data->proposal_id
                            ));
                    } //$abstract_data = $abstracts_q->fetchObject()
                    drupal_set_message(t('Approved step no. ' . $step_data->number), 'status');
                }
                    // email 
                    /*$email_subject = t('[!site_name][power system simulation Project] Your uploaded power system simulation project have been approved', array(
                        '!site_name' => variable_get('site_name', '')
                    ));
                    $email_body = array(
                        0 => t('

Dear !user_name,

Congratulations!
Your simulation and abstract for Power Systems Simulation Project at FOSSEE with the following details have been approved.

Full Name: ' . $user_info->name_title . ' ' . $user_info->contributor_name . '
Email : ' . $user_data->mail . '
University/Institute : ' . $user_info->university . '
City : ' . $user_info->city . '

Project Title  : ' . $user_info->project_title . '
Description of the simulation: ' . $user_info->description .'

Kindly send us the internship forms as early as possible for processing your honorarium on time. In case you have already sent these forms, please share the the consignment number or tracking id with us.

Note: It will take upto 30 days from the time we receive your forms, to process your honorarium.


Best Wishes,

!site_name Power Systems Team,
FOSSEE,IIT Bombay', array(
                            '!site_name' => variable_get('site_name', ''),
                            '!user_name' => $user_data->name
                        ))
                    );
                    /** sending email when everything done *
                    $email_to = $user_data->mail;
                    $from = variable_get('om_pssp_from_email', '');
                    $bcc = variable_get('om_pssp_emails', '');
                    $cc = variable_get('om_pssp_cc_emails', '');
                    $params['standard']['subject'] = $email_subject;
                    $params['standard']['body'] = $email_body;
                    $params['standard']['headers'] = array(
                        'From' => $from,
                        'MIME-Version' => '1.0',
                        'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
                        'Content-Transfer-Encoding' => '8Bit',
                        'X-Mailer' => 'Drupal',
                        'Cc' => $cc,
                        'Bcc' => $bcc
                    );
                    if (!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE))
                    {
                        $msg = drupal_set_message('Error sending email message.', 'error');
                    }*/ //!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE)
                } //$form_state['values']['om_pssp_actions'] == 1
                elseif ($form_state['values']['step_actions'] == 2)
                {
                    /*if (strlen(trim($form_state['values']['message'])) <= 30)
                    {
                        form_set_error('message', t(''));
                        $msg = drupal_set_message("Please mention the reason for marking resubmit. Minimum 30 character required", 'error');
                        return $msg;
                    }*/
                    //pending review entire project 

                    $query = db_select('arduino_projects_blog_step');
                    $query->fields('arduino_projects_blog_step');
                    $query->condition('number', $form_state['values']['step']);
                    $result = $query->execute();
                    $step_data = $result->fetchObject();
                    $query = db_select('arduino_projects_blog_step_information');
                    $query->fields('arduino_projects_blog_step_information');
                    $query->condition('step_id', $step_data->id);
                    $abstracts_q = $query->execute();
                    $experiment_list = '';
                    while ($abstract_data = $abstracts_q->fetchObject())
                    {
                        db_query("UPDATE {arduino_projects_blog_step_information} SET approval_status = 0, approver_uid = :approver_uid, approval_date = :approval_date WHERE id = :id", array(
                            ':approver_uid' => $user->uid,
                            ':approval_date' => time(),
                            ':id' => $abstract_data->id
                        ));
                        db_query("UPDATE {arduino_projects_blog_step} SET approval_status = 0, approver_uid = :approver_uid, approval_date = :approval_date WHERE id = :id", array(
                            ':approver_uid' => $user->uid,
                            ':approval_date' => time(),
                            ':id' => $abstract_data->step_id
                        ));
                        db_query("UPDATE {arduino_projects_blog_proposal} SET submitted_steps = 0 WHERE id = :proposal_id", array(
                            ':proposal_id' => $step_data->proposal_id
                            ));
                    } //$abstract_data = $abstracts_q->fetchObject()
                    drupal_set_message(t('Resubmit the step files'), 'status');
                    // email 
                    /*$email_subject = t('[!site_name][power system simulation Project] Your uploaded power system simulation project have been marked as pending', array(
                        '!site_name' => variable_get('site_name', '')
                    ));
                    $email_body = array(
                        0 => t('

Dear !user_name,

Kindly resubmit the project files for the project: ' . $user_info->project_title . '.
Description of the simulation: ' . $user_info->description . '

Reason: ' . $form_state['values']['message'] . '

Looking forward for the re-submission from you with the above suggested changes.

Best Wishes,

!site_name Power Systems Team,
FOSSEE,IIT Bombay', array(
                            '!site_name' => variable_get('site_name', ''),
                            '!user_name' => $user_data->name
                        ))
                    );
                    /** sending email when everything done *
                    $email_to = $user_data->mail;
                    $from = variable_get('om_pssp_from_email', '');
                    $bcc = variable_get('om_pssp_emails', '');
                    $cc = variable_get('om_pssp_cc_emails', '');
                    $params['standard']['subject'] = $email_subject;
                    $params['standard']['body'] = $email_body;
                    $params['standard']['headers'] = array(
                        'From' => $from,
                        'MIME-Version' => '1.0',
                        'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
                        'Content-Transfer-Encoding' => '8Bit',
                        'X-Mailer' => 'Drupal',
                        'Cc' => $cc,
                        'Bcc' => $bcc
                    );
                    if (!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE))
                    {
                        drupal_set_message('Error sending email message.', 'error');
                    } *///!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE)
                } //$form_state['values']['om_pssp_actions'] == 2
                elseif ($form_state['values']['om_pssp_actions'] == 3) //disapprove and delete entire power system simulation project
                {
                    if (strlen(trim($form_state['values']['message'])) <= 30)
                    {
                        form_set_error('message', t(''));
                        $msg = drupal_set_message("Please mention the reason for disapproval. Minimum 30 character required", 'error');
                        return $msg;
                    } //strlen(trim($form_state['values']['message'])) <= 30
                    if (!user_access('om pssp bulk delete code'))
                    {
                        $msg = drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Lab.'), 'error');
                        return $msg;
                    } //!user_access('om_pssp bulk delete code')
                    if (om_pssp_abstract_delete_project($form_state['values']['om_pssp_project'])) //////
                    {
                        drupal_set_message(t('Dis-Approved and Deleted Entire power system simulation project.'), 'status');
                        $email_subject = t('[!site_name][power system simulation Project] Your uploaded power system simulation project have been marked as dis-approved', array(
                        '!site_name' => variable_get('site_name', '')
                    ));
                    $email_body = array(
                        0 => t('
Dear !user_name,

We regret to inform you that your simulation and abstract for Power Systems Simulation Project at FOSSEE with the following details have been disapproved:

Full Name: ' . $user_info->name_title . ' ' . $user_info->contributor_name . '
Email : ' . $user_data->mail . '
University/Institute : ' . $user_info->university . '
City : ' . $user_info->city . '

Project Title  : ' . $user_info->project_title . '
Description of the simulation: ' . $user_info->description .'

Reason for dis-approval: ' . $form_state['values']['message'] . '

Kindly note that the incorrect files will be deleted from all our databases.

Thank you for participating in the Power Systems Simulation Project. You are welcome to submit a new proposal.

Best Wishes,

!site_name Power Systems Team,
FOSSEE,IIT Bombay', array(
                        '!site_name' => variable_get('site_name', ''),
                        '!user_name' => $user_data->name
                                            ))
                    );
                    $email_to = $user_data->mail;
                    $from = variable_get('om_pssp_from_email', '');
                    $bcc = variable_get('om_pssp_emails', '');
                    $cc = variable_get('om_pssp_cc_emails', '');
                    $params['standard']['subject'] = $email_subject;
                    $params['standard']['body'] = $email_body;
                    $params['standard']['headers'] = array(
                        'From' => $from,
                        'MIME-Version' => '1.0',
                        'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
                        'Content-Transfer-Encoding' => '8Bit',
                        'X-Mailer' => 'Drupal',
                        'Cc' => $cc,
                        'Bcc' => $bcc
                    );
                    if (!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE))
                    {
                        drupal_set_message('Error sending email message.', 'error');
                    }
                    } //om_pssp_abstract_delete_project($form_state['values']['om_pssp_project'])
                    else
                    {
                        drupal_set_message(t('Error Dis-Approving and Deleting Entire power system simulation project.'), 'error');
                    }
                    // email 
                    
                } //$form_state['values']['om_pssp_actions'] == 3
            } //user_access('om_pssp project bulk manage code')
        return $msg;
    } //$form_state['clicked_button']['#value'] == 'Submit'
}
function _step_list_of_projects()
{
    $project_titles_list = array(
        '0' => 'Please select...'
    );
    /*$book_titles_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE approval_status = 1 OR approval_status = 3 ORDER BY book ASC");*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('submitted_steps', 1);
    $query->condition('approval_status', 1);
    $query->orderBy('project_title', 'ASC');
    $project_title_q = $query->execute();
    while ($project_title_data = $project_title_q->fetchObject()) {
        $project_titles_list[$project_title_data->id] = $project_title_data->project_title;
    }
    return $project_titles_list;
}
function _ajax_bulk_get_step_list($proposal_id = 0)
{
    $steps_list = array(
        '0' => 'Please select...'
    );
    /*$book_chapters_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d ORDER BY number ASC", $preference_id);*/
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('proposal_id', $proposal_id);
    $query->condition('approval_status', 0);
    $query->orderBy('number', 'ASC');
    $steps_q = $query->execute();
    while ($steps_data = $steps_q->fetchObject()) {
        $steps_list[$steps_data->number] = $steps_data->number . '. ' . $steps_data->name;
    }
    return $steps_list;
}
/*function _ajax_bulk_get_examples($chapter_id = 0)
{
    $book_examples = array(
        '0' => 'Please select...'
    );
    /*$book_examples_q = db_query("SELECT * FROM {textbook_companion_example} WHERE chapter_id = %d ORDER BY
    CAST(SUBSTRING_INDEX(number, '.', 1) AS BINARY) ASC,
    CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', 2), '.', -1) AS UNSIGNED) ASC,
    CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', -1), '.', 1) AS UNSIGNED) ASC", $chapter_id);
    $query = db_select('textbook_companion_example');
    $query->fields('textbook_companion_example');
    $query->condition('chapter_id', $chapter_id);
    //$query->orderBy('CAST', 'ASC');
    //$query->orderBy('CAST', 'ASC');
    //$query->orderBy('CAST', 'ASC');
    $book_examples_q = $query->execute();
    while ($book_examples_data = $book_examples_q->fetchObject()) {
        $book_examples[$book_examples_data->id] = $book_examples_data->number . ' (' . $book_examples_data->caption . ')';
    }
    return $book_examples;
}*/
function _bulk_list_book_actions()
{
    $book_actions = array(
        '0' => 'Please select...'
    );
    $book_actions[1] = 'Approve Entire Book';
    $book_actions[2] = 'Pending Review Entire Book';
    $book_actions[3] = 'Dis-Approve Entire Book (This will delete all the examples in the book)';
    $book_actions[4] = 'Delete Entire Book Including Proposal';
    return $book_actions;
}
function _bulk_list_step_actions()
{
    $chapter_actions = array(
        '0' => 'Please select...'
    );
    $chapter_actions[1] = 'Approve Step';
    $chapter_actions[2] = 'Resubmit the step';
    //$chapter_actions[3] = 'Dis-Approve Step (This will delete all the examples in the chapter)';
    return $chapter_actions;
}
function _bulk_list_example_actions()
{
    $example_actions = array(
        '0' => 'Please select...'
    );
    $example_actions[1] = 'Approve  Approve Example';
    $example_actions[2] = 'Pending Review Example';
    $example_actions[3] = 'Dis-approve Example (This will delete the example)';
    return $example_actions;
}
/****************************** Ajax Callback function ***************************/
function ajax_bulk_step_list_callback($form, $form_state)
{
    $commands = array();
    $project_default_value = $form_state['values']['project_title'];
    if ($project_default_value > 0) {
        /*$commands[] = ajax_command_html('#ajax_selected_book_notes', l('Notes for Reviewers', 'code_approval/notes/' . $project_default_value));*/
        $form['book_actions']['#options'] = _bulk_list_book_actions();
        $commands[] = ajax_command_replace('#ajax_selected_book_action', drupal_render($form['book_actions']));
        $form['step']['#options'] = _ajax_bulk_get_step_list($project_default_value);
        $commands[] = ajax_command_replace('#ajax_select_step_list', drupal_render($form['step']));
        $commands[] = ajax_command_html('#ajax_download_chapter', '');
        $form['chapter_actions']['#options'] = _bulk_list_book_actions();
        $commands[] = ajax_command_replace('#ajax_selected_chapter_action', drupal_render($form['chapter_actions']));
        $commands[] = ajax_command_html('#ajax_selected_chapter_action', '');
        $commands[] = ajax_command_html('#ajax_selected_example', '');
        $form['example_actions']['#options'] = _bulk_list_example_actions();
        $commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
        $commands[] = ajax_command_html('#ajax_selected_example_action', '');
        $commands[] = ajax_command_html('#ajax_download_selected_example', '');
        $commands[] = ajax_command_html('#ajax_edit_selected_example', '');
        $form['example_files']['#title'] = '';
        $form['example_files']['#markup'] = '';
        $commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
    } else {
        $commands[] = ajax_command_html('#ajax_selected_book', '');
        $commands[] = ajax_command_html('#ajax_selected_book_pdf', '');
        $commands[] = ajax_command_html('#ajax_selected_book_regenerate_pdf', '');
        $commands[] = ajax_command_html('#ajax_selected_book_notes', '');
        $form['step']['#options'] = _ajax_bulk_get_step_list();
        $commands[] = ajax_command_replace('#ajax_select_step_list', drupal_render($form['step']));
        $commands[] = ajax_command_html('#ajax_select_step_list', '');
        $form['book_actions']['#options'] = _bulk_list_book_actions();
        $commands[] = ajax_command_replace('#ajax_selected_book_action', drupal_render($form['book_actions']));
        $commands[] = ajax_command_html('#ajax_selected_book_action', '');
        $form['step_actions']['#options'] = _bulk_list_step_actions();
        $commands[] = ajax_command_replace('#ajax_selected_step_action', drupal_render($form['step_actions']));
        $commands[] = ajax_command_html('#ajax_selected_chapter_action', '');
        $commands[] = ajax_command_html('#ajax_download_chapter', '');
        $commands[] = ajax_command_html('#ajax_selected_example', '');
        $form['example_actions']['#options'] = _bulk_list_example_actions();
        $commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
        $commands[] = ajax_command_html('#ajax_selected_example_action', '');
        $commands[] = ajax_command_html('#ajax_download_selected_example', '');
        $commands[] = ajax_command_html('#ajax_edit_selected_example', '');
        $form['example_files']['#title'] = '';
        $form['example_files']['#markup'] = '';
        $commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
    }
    return array(
        '#type' => 'ajax',
        '#commands' => $commands
    );
}
function ajax_bulk_step_details_list_callback($form, $form_state)
{
    $commands = array();
    $step_default_value = $form_state['values']['project_title'];
    //var_dump($step_default_value);die;
    if ($step_default_value != 0)
    {
        $commands[] = ajax_command_html('#ajax_selected_step', _step_details($step_default_value));
        $form['om_pssp_actions']['#options'] = _bulk_list_step_actions();
        $commands[] = ajax_command_replace('#ajax_selected_step_action', drupal_render($form['step_actions']));
    } //$step_default_value != 0
    else
    {
        $commands[] = ajax_command_html('#ajax_selected_step', '');
        $commands[] = ajax_command_data('#ajax_selected_step', 'form_state_value_select', $form_state['values']['step']);
    }
    return array(
        '#type' => 'ajax',
        '#commands' => $commands
    );
}
function ajax_bulk_example_files_callback($form, $form_state)
{
    $commands = array();
    $example_list_default_value = $form_state['values']['example'];
    //var_dump($example_list_default_value);
    if ($example_list_default_value > 0) {
        /*************************************************************************************/
        /*$example_list_q = db_query("SELECT * FROM {textbook_companion_example_files} WHERE example_id = %d", $form_state['values']['example']);*/
        $query = db_select('textbook_companion_example_files');
        $query->fields('textbook_companion_example_files');
        $query->condition('example_id', $example_list_default_value);
        $example_list_q = $query->execute();
        if ($example_list_q) {
            $example_files_rows = array();
            while ($example_list_data = $example_list_q->fetchObject()) {
                $example_file_type = '';
                switch ($example_list_data->filetype) {
                    case 'S':
                        $example_file_type = 'Source or Main file';
                        break;
                    case 'R':
                        $example_file_type = 'Result file';
                        break;
                    case 'X':
                        $example_file_type = 'xcos file';
                        break;
                    default:
                        $example_file_type = 'Unknown';
                        break;
                }
                $example_files_rows[] = array(
                    l($example_list_data->filename, 'arduino-projects/download/file/' . $example_list_data->id),
                    $example_file_type
                );
            }
            /* dependency files */
            /*$dependency_list_q = db_query("SELECT dependency.id as dependency_id, dependency.filename as dependency_filename, dependency.caption as dependency_caption
            FROM {textbook_companion_example_dependency} example_dependency LEFT JOIN {textbook_companion_dependency_files} dependency
            ON example_dependency.dependency_id = dependency.id
            WHERE example_dependency.example_id = %d", $form_state['values']['example']);*/
            /*
            $query = db_select('textbook_companion_example_dependency', 'example_dependency');
            $query->fields('dependency', array('id', 'filename', 'caption'));
            $query->addField('dependency','id','dependency_id');
            $query->addField('dependency','filename','dependency_filename');
            $query->addField('dependency','caption','dependency_caption');
            $query->leftJoin('textbook_companion_dependency_files', 'dependency', 'example_dependency.dependency_id = dependency.id');
            $query->condition('example_dependency.example_id',  $form_state['values']['example']);
            $dependency_list_q = $query->execute();
            
            while ($dependency_list_data = $dependency_list_q->fetchObject())
            {
            $example_file_type = 'Dependency file';
            $temp_caption = '';
            if ($dependency_list_data->dependency_caption)
            $temp_caption = ' (' . $dependency_list_data->dependency_caption . ')';
            $example_files_rows[] = array(l($dependency_list_data->dependency_filename, 'download/dependency/' . $dependency_list_data->dependency_id) . $temp_caption, $example_file_type);
            }
            */
            /* creating list of files table */
            $example_files_header = array(
                'Filename',
                'Type'
            );
            $example_files = theme('table', array(
                'header' => $example_files_header,
                'rows' => $example_files_rows
            ));
            $form['example_files']['#title'] = 'List of example files';
            $form['example_files']['#markup'] = $example_files;
            $commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
            $commands[] = ajax_command_html('#ajax_download_selected_example', l('Download Example', 'arduino-projects/download/example/' . $example_list_default_value));
            $commands[] = ajax_command_html('#ajax_edit_selected_example', l('Edit Example', 'arduino-projects/code-approval/editcode/' . $example_list_default_value));
            $form['example_actions']['#options'] = _bulk_list_example_actions();
            $commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
            //$commands[] = ajax_command_html('#ajax_selected_example_action', '' );
        }
    } else {
        $commands[] = ajax_command_html('#ajax_download_selected_example', '');
        $commands[] = ajax_command_html('#ajax_edit_selected_example', '');
        $form['example_files']['#title'] = '';
        $form['example_files']['#markup'] = '';
        $commands[] = ajax_command_replace('#ajax_example_files_list', drupal_render($form['example_files']));
        $form['example_actions']['#options'] = _bulk_list_example_actions();
        $commands[] = ajax_command_replace('#ajax_selected_example_action', drupal_render($form['example_actions']));
        $commands[] = ajax_command_html('#ajax_selected_example_action', '');
    }
    return array(
        '#type' => 'ajax',
        '#commands' => $commands
    );
}
function ajax_bulk_chapter_actions_callback()
{
    //if($form_state['values']['chapter_actions'] > 0){
    //	$form['book_actions']['#options'] = _bulk_list_book_actions();
    //$commands[] = ajax_command_replace('#ajax_selected_book_action',drupal_render($form['book_actions']));
    //	}
    return array(
        '#type' => 'ajax',
        '#commands' => $commands
    );
}

function _step_details($step_id)
{
    $return_html = "";
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('id', $step_id);
    $proposal_data = $query->execute()->fetchObject();
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('proposal_id', $proposal_data->id);
    $query->condition('approval_status', 0);
    $step_data = $query->execute()->fetchObject();
    //var_dump($step_data);die;
    
    $query = db_select('arduino_projects_blog_step_information');
    $query->fields('arduino_projects_blog_step_information');
    $query->condition('step_id', $step_data->id);
    $step_info_data = $query->execute()->fetchObject();
    //var_dump($abstracts_pro);die;
    $query = db_select('arduino_projects_blog_step_files');
    $query->fields('arduino_projects_blog_step_files');
    $query->condition('step_id', $step_data->id);
    $query->condition('filetype', 'I');
    $step_images = $query->execute()->fetchObject();
    $images_count = $query->execute()->rowCount();
    $query = db_select('arduino_projects_blog_step_files');
    $query->fields('arduino_projects_blog_step_files');
    $query->condition('step_id', $step_data->id);
    $query->condition('filetype', 'G');
    $step_gifs = $query->execute()->fetchObject();
    $gifs_count = $query->execute()->rowCount();
    if(!$step_info_data->video){
        $video = 'Not uploaded';
    }
    else{
        $video = convertLinkToEmbed($step_info_data->video, 450, 350);
    }
    if($images_count > 0){
        $download_step_images = l('Download Images','arduino-projects/download/step-images/' . $step_id);
    }
    else{
        $download_step_images = 'Not uploaded';
    }
    if($gifs_count > 0){
        $download_step_gifs = l('Download Gifs','arduino-projects/download/step-gifs/' . $step_id);
    }
    else{
        $download_step_gifs = 'Not uploaded';
    }
    $return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->contributor_name . '<br /><br />';
    $return_html .= '<strong>Title of the Project:</strong><br />' . $proposal_data->project_title . '<br /><br />';
    $return_html .= '<strong>Step name:</strong><br />' . $step_data->name . '<br /><br />';
    $return_html .= '<strong>Step description:</strong><br />' . $step_info_data->description;
    $return_html .= '<strong>Video</strong><br />' . $video . '<br /><br />';
    /*$return_html .= '<strong>Uploaded an abstract (brief outline) of the project:</strong><br />' . $abstract_filename . '<br /><br />';
    $return_html .= '<strong>Upload the  power system simulation for the developed process:</strong><br />' . $abstracts_query_process_filename . '<br /><br />';*/
    $return_html .= '<strong>Images uploaded for this step</strong><br />' . $download_step_images . '<br /><br />';
    $return_html .= '<strong>Gifs uploaded for this step</strong><br />' . $download_step_gifs;
    return $return_html;
}

function bulk_approval_form($form, &$form_state)
{
    $options_first = _bulk_list_of_project_titles();
    $selected = isset($form_state['values']['project_title']) ? $form_state['values']['project_title'] : key($options_first);
    $form = array();
    $form['project_title'] = array(
        '#type' => 'select',
        '#title' => t('Title of the power system simulation project'),
        '#options' => _bulk_list_of_project_titles(),
        '#default_value' => $selected,
        '#ajax' => array(
            
            'callback' => 'ajax_bulk_om_pssp_abstract_details_callback'
        ),
        '#suffix' => '<div id="ajax_selected_om_pssp"></div><div id="ajax_selected_om_pssp_pdf"></div>'
    );
    $form['project_actions'] = array(
        '#type' => 'select',
        '#title' => t('Please select action for the project'),
        '#options' => _bulk_list_project_actions(),
        '#default_value' => 0,
        '#prefix' => '<div id="ajax_selected_om_pssp_action" style="color:red;">',
        '#suffix' => '</div>',
        '#states' => array(
            'invisible' => array(
                ':input[name="om_pssp_project"]' => array(
                    'value' => 0
                )
            )
        )
    );
    $form['message'] = array(
        '#type' => 'textarea',
        '#title' => t('Please specify the reason for marking resubmit/disapproval'),
        '#prefix' => '<div id= "message_submit">',
        '#states' => array(
            'visible' => array(
                array(
                    ':input[name="project_actions"]' => array(
                        'value' => 3
                    )
                ),
                'or',
                array(
                    ':input[name="project_actions"]' => array(
                        'value' => 2
                    )
                )
            ),
        'required' => array(
                array(
                    array(
                        ':input[name="project_actions"]' => array(
                            'value' => 2
                        )
                    ),
                    'or',
                    array(
                        ':input[name="project_actions"]' => array(
                            'value' => 3
                        )
                    ),
                )
            )
        ),
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
        '#states' => array(
            'invisible' => array(
                ':input[name="lab"]' => array(
                    'value' => 0
                )
            )
        )
    );
    return $form;
}
function ajax_bulk_om_pssp_abstract_details_callback($form, $form_state)
{
    $commands = array();
    $om_pssp_project_default_value = $form_state['values']['om_pssp_project'];
    if ($om_pssp_project_default_value != 0)
    {
        //$commands[] = ajax_command_html('#ajax_selected_om_pssp', _om_pssp_details($om_pssp_project_default_value));
        $form['om_pssp_actions']['#options'] = _bulk_list_project_actions();
        $commands[] = ajax_command_replace('#ajax_selected_om_pssp_action', drupal_render($form['om_pssp_actions']));
    } //$om_pssp_project_default_value != 0
    else
    {
        $commands[] = ajax_command_html('#ajax_selected_om_pssp', '');
        $commands[] = ajax_command_data('#ajax_selected_om_pssp', 'form_state_value_select', $form_state['values']['om_pssp_project']);
    }
    return array(
        '#type' => 'ajax',
        '#commands' => $commands
    );
}
/************************************************************/
function om_pssp_abstract_bulk_approval_form_submit($form, &$form_state)
{
    global $user;
    $msg = '';
    $root_path = om_pssp_path();
    if ($form_state['clicked_button']['#value'] == 'Submit')
    {
        if ($form_state['values']['om_pssp_project'])
        // om_pssp_abstract_del_lab_pdf($form_state['values']['om_pssp_project']);
            if (user_access('om pssp bulk manage abstract'))
            {
                $query = db_select('om_pssp_proposal');
                $query->fields('om_pssp_proposal');
                $query->condition('id', $form_state['values']['om_pssp_project']);
                $user_query = $query->execute();
                $user_info = $user_query->fetchObject();
                $user_data = user_load($user_info->uid);
                if ($form_state['values']['om_pssp_actions'] == 1)
                {
                    // approving entire project //
                    $query = db_select('om_pssp_submitted_abstracts');
                    $query->fields('om_pssp_submitted_abstracts');
                    $query->condition('proposal_id', $form_state['values']['om_pssp_project']);
                    $abstracts_q = $query->execute();
                    $experiment_list = '';
                    while ($abstract_data = $abstracts_q->fetchObject())
                    {
                        db_query("UPDATE {om_pssp_submitted_abstracts} SET abstract_approval_status = 1, is_submitted = 1, approver_uid = :approver_uid WHERE id = :id", array(
                            ':approver_uid' => $user->uid,
                            ':id' => $abstract_data->id
                        ));
                        db_query("UPDATE {om_pssp_submitted_abstracts_file} SET file_approval_status = 1, approvar_uid = :approver_uid WHERE submitted_abstract_id = :submitted_abstract_id", array(
                            ':approver_uid' => $user->uid,
                            ':submitted_abstract_id' => $abstract_data->id
                        ));
                    } //$abstract_data = $abstracts_q->fetchObject()
                    drupal_goto('powersystems/pssp/manage-proposal/all');
                    drupal_set_message(t('Approved power system simulation project.'), 'status');
                    // email 
                    $email_subject = t('[!site_name][power system simulation Project] Your uploaded power system simulation project have been approved', array(
                        '!site_name' => variable_get('site_name', '')
                    ));
                    $email_body = array(
                        0 => t('

Dear !user_name,

Congratulations!
Your simulation and abstract for Power Systems Simulation Project at FOSSEE with the following details have been approved.

Full Name: ' . $user_info->name_title . ' ' . $user_info->contributor_name . '
Email : ' . $user_data->mail . '
University/Institute : ' . $user_info->university . '
City : ' . $user_info->city . '

Project Title  : ' . $user_info->project_title . '
Description of the simulation: ' . $user_info->description .'

Kindly send us the internship forms as early as possible for processing your honorarium on time. In case you have already sent these forms, please share the the consignment number or tracking id with us.

Note: It will take upto 30 days from the time we receive your forms, to process your honorarium.


Best Wishes,

!site_name Power Systems Team,
FOSSEE,IIT Bombay', array(
                            '!site_name' => variable_get('site_name', ''),
                            '!user_name' => $user_data->name
                        ))
                    );
                    /** sending email when everything done **/
                    $email_to = $user_data->mail;
                    $from = variable_get('om_pssp_from_email', '');
                    $bcc = variable_get('om_pssp_emails', '');
                    $cc = variable_get('om_pssp_cc_emails', '');
                    $params['standard']['subject'] = $email_subject;
                    $params['standard']['body'] = $email_body;
                    $params['standard']['headers'] = array(
                        'From' => $from,
                        'MIME-Version' => '1.0',
                        'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
                        'Content-Transfer-Encoding' => '8Bit',
                        'X-Mailer' => 'Drupal',
                        'Cc' => $cc,
                        'Bcc' => $bcc
                    );
                    if (!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE))
                    {
                        $msg = drupal_set_message('Error sending email message.', 'error');
                    } //!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE)
                } //$form_state['values']['om_pssp_actions'] == 1
                elseif ($form_state['values']['om_pssp_actions'] == 2)
                {
                    if (strlen(trim($form_state['values']['message'])) <= 30)
                    {
                        form_set_error('message', t(''));
                        $msg = drupal_set_message("Please mention the reason for marking resubmit. Minimum 30 character required", 'error');
                        return $msg;
                    }
                    //pending review entire project 
                    $query = db_select('om_pssp_submitted_abstracts');
                    $query->fields('om_pssp_submitted_abstracts');
                    $query->condition('proposal_id', $form_state['values']['om_pssp_project']);
                    $abstracts_q = $query->execute();
                    $experiment_list = '';
                    while ($abstract_data = $abstracts_q->fetchObject())
                    {
                        db_query("UPDATE {om_pssp_submitted_abstracts} SET abstract_approval_status = 0, is_submitted = 0, approver_uid = :approver_uid WHERE id = :id", array(
                            ':approver_uid' => $user->uid,
                            ':id' => $abstract_data->id
                        ));
                        db_query("UPDATE {om_pssp_proposal} SET is_submitted = 0, approver_uid = :approver_uid WHERE id = :id", array(
                            ':approver_uid' => $user->uid,
                            ':id' => $abstract_data->proposal_id
                        ));
                        db_query("UPDATE {om_pssp_submitted_abstracts_file} SET file_approval_status = 0, approvar_uid = :approver_uid WHERE submitted_abstract_id = :submitted_abstract_id", array(
                            ':approver_uid' => $user->uid,
                            ':submitted_abstract_id' => $abstract_data->id
                        ));
                    } //$abstract_data = $abstracts_q->fetchObject()
                    drupal_set_message(t('Resubmit the project files'), 'status');
                    // email 
                    $email_subject = t('[!site_name][power system simulation Project] Your uploaded power system simulation project have been marked as pending', array(
                        '!site_name' => variable_get('site_name', '')
                    ));
                    $email_body = array(
                        0 => t('

Dear !user_name,

Kindly resubmit the project files for the project: ' . $user_info->project_title . '.
Description of the simulation: ' . $user_info->description . '

Reason: ' . $form_state['values']['message'] . '

Looking forward for the re-submission from you with the above suggested changes.

Best Wishes,

!site_name Power Systems Team,
FOSSEE,IIT Bombay', array(
                            '!site_name' => variable_get('site_name', ''),
                            '!user_name' => $user_data->name
                        ))
                    );
                    /** sending email when everything done **/
                    $email_to = $user_data->mail;
                    $from = variable_get('om_pssp_from_email', '');
                    $bcc = variable_get('om_pssp_emails', '');
                    $cc = variable_get('om_pssp_cc_emails', '');
                    $params['standard']['subject'] = $email_subject;
                    $params['standard']['body'] = $email_body;
                    $params['standard']['headers'] = array(
                        'From' => $from,
                        'MIME-Version' => '1.0',
                        'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
                        'Content-Transfer-Encoding' => '8Bit',
                        'X-Mailer' => 'Drupal',
                        'Cc' => $cc,
                        'Bcc' => $bcc
                    );
                    if (!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE))
                    {
                        drupal_set_message('Error sending email message.', 'error');
                    } //!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE)
                } //$form_state['values']['om_pssp_actions'] == 2
                elseif ($form_state['values']['om_pssp_actions'] == 3) //disapprove and delete entire power system simulation project
                {
                    if (strlen(trim($form_state['values']['message'])) <= 30)
                    {
                        form_set_error('message', t(''));
                        $msg = drupal_set_message("Please mention the reason for disapproval. Minimum 30 character required", 'error');
                        return $msg;
                    } //strlen(trim($form_state['values']['message'])) <= 30
                    if (!user_access('om pssp bulk delete code'))
                    {
                        $msg = drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Lab.'), 'error');
                        return $msg;
                    } //!user_access('om_pssp bulk delete code')
                    if (om_pssp_abstract_delete_project($form_state['values']['om_pssp_project'])) //////
                    {
                        drupal_set_message(t('Dis-Approved and Deleted Entire power system simulation project.'), 'status');
                        $email_subject = t('[!site_name][power system simulation Project] Your uploaded power system simulation project have been marked as dis-approved', array(
                        '!site_name' => variable_get('site_name', '')
                    ));
                    $email_body = array(
                        0 => t('
Dear !user_name,

We regret to inform you that your simulation and abstract for Power Systems Simulation Project at FOSSEE with the following details have been disapproved:

Full Name: ' . $user_info->name_title . ' ' . $user_info->contributor_name . '
Email : ' . $user_data->mail . '
University/Institute : ' . $user_info->university . '
City : ' . $user_info->city . '

Project Title  : ' . $user_info->project_title . '
Description of the simulation: ' . $user_info->description .'

Reason for dis-approval: ' . $form_state['values']['message'] . '

Kindly note that the incorrect files will be deleted from all our databases.

Thank you for participating in the Power Systems Simulation Project. You are welcome to submit a new proposal.

Best Wishes,

!site_name Power Systems Team,
FOSSEE,IIT Bombay', array(
                        '!site_name' => variable_get('site_name', ''),
                        '!user_name' => $user_data->name
                                            ))
                    );
                    $email_to = $user_data->mail;
                    $from = variable_get('om_pssp_from_email', '');
                    $bcc = variable_get('om_pssp_emails', '');
                    $cc = variable_get('om_pssp_cc_emails', '');
                    $params['standard']['subject'] = $email_subject;
                    $params['standard']['body'] = $email_body;
                    $params['standard']['headers'] = array(
                        'From' => $from,
                        'MIME-Version' => '1.0',
                        'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
                        'Content-Transfer-Encoding' => '8Bit',
                        'X-Mailer' => 'Drupal',
                        'Cc' => $cc,
                        'Bcc' => $bcc
                    );
                    if (!drupal_mail('om_pssp', 'standard', $email_to, language_default(), $params, $from, TRUE))
                    {
                        drupal_set_message('Error sending email message.', 'error');
                    }
                    } //om_pssp_abstract_delete_project($form_state['values']['om_pssp_project'])
                    else
                    {
                        drupal_set_message(t('Error Dis-Approving and Deleting Entire power system simulation project.'), 'error');
                    }
                    // email 
                    
                } //$form_state['values']['om_pssp_actions'] == 3
            } //user_access('om_pssp project bulk manage code')
        return $msg;
    } //$form_state['clicked_button']['#value'] == 'Submit'
}
/**********************************************************/
function _bulk_list_of_project_titles()
{

    $project_titles = array(
        '0' => 'Please select...'
    );
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('submitted_steps', 1);
    $query->condition('approval_status', 1);
    $query->orderBy('project_title', 'ASC');
    $project_titles_q = $query->execute();
    while ($project_titles_data = $project_titles_q->fetchObject())
    {
        $query_step = db_select('arduino_projects_blog_step');
        $query_step->fields('arduino_projects_blog_step');
        $query_step->condition('proposal_id', $project_titles_data->id);
        $query_step->condition('approval_status', 1);
        $query_step_count = $query_step->execute()->rowCount();
        if($query_step_count == $project_titles_data->no_of_steps_approved){
        $project_titles[$project_titles_data->id] = $project_titles_data->project_title . ' (Proposed by ' . $project_titles_data->contributor_name . ')';
        }
    } //$project_titles_data = $project_titles_q->fetchObject()
    return $project_titles;
}
function _bulk_list_project_actions()
{
    $om_pssp_actions = array(
        0 => 'Please select...'
    );
    $om_pssp_actions[1] = 'Approve Entire Project';
    $om_pssp_actions[2] = 'Resubmit All Project files';
    $om_pssp_actions[3] = 'Dis-Approve Entire Project (This will delete the Project)';
    //$om_pssp_actions[4] = 'Delete Entire power system simulation Project Including Proposal';
    return $om_pssp_actions;
}
function _om_pssp_details($om_pssp_proposal_id)
{
    $return_html = "";
    $query_pro = db_select('om_pssp_proposal');
    $query_pro->fields('om_pssp_proposal');
    $query_pro->condition('id', $om_pssp_proposal_id);
    $abstracts_pro = $query_pro->execute()->fetchObject();
    //var_dump($abstracts_pro);die;
    $query_pdf = db_select('om_pssp_submitted_abstracts_file');
    $query_pdf->fields('om_pssp_submitted_abstracts_file');
    $query_pdf->condition('proposal_id', $om_pssp_proposal_id);
    $query_pdf->condition('filetype', 'A');
    $abstracts_pdf = $query_pdf->execute()->fetchObject();
    if ($abstracts_pdf == TRUE)
    {
        if ($abstracts_pdf->filename != "NULL" || $abstracts_pdf->filename != "")
        {
            $abstract_filename = $abstracts_pdf->filename;
        } //$abstracts_pdf->filename != "NULL" || $abstracts_pdf->filename != ""
        else
        {
            $abstract_filename = "File not uploaded";
        }
    } //$abstracts_pdf == TRUE
    else
    {
        $abstract_filename = "File not uploaded";
    }
    $query_process = db_select('om_pssp_submitted_abstracts_file');
    $query_process->fields('om_pssp_submitted_abstracts_file');
    $query_process->condition('proposal_id', $om_pssp_proposal_id);
    $query_process->condition('filetype', 'S');
    $abstracts_query_process = $query_process->execute()->fetchObject();
    $query = db_select('om_pssp_submitted_abstracts');
    $query->fields('om_pssp_submitted_abstracts');
    $query->condition('proposal_id', $om_pssp_proposal_id);
    $abstracts_q = $query->execute()->fetchObject();
    if($abstracts_q->other_libraries_used == 1)
    {
        $other_libraries_used = "Yes";
    }
    else
    {
        $other_libraries_used = "No";
    }
    if ($abstracts_q)
    {
        if ($abstracts_q->is_submitted == 0)
        {
            //drupal_set_message(t('Abstract is not submmited yet.'), 'error', $repeat = FALSE);
            //return;
        } //$abstracts_q->is_submitted == 0
    } //$abstracts_q
    //var_dump($abstracts_query_process);die;
    if ($abstracts_query_process == TRUE)
    {
        if ($abstracts_query_process->filename != "NULL" || $abstracts_query_process->filename != "")
        {
            $abstracts_query_process_filename = $abstracts_query_process->filename;
        } //$abstracts_query_process->filename != "NULL" || $abstracts_query_process->filename != ""
        else
        {
            $abstracts_query_process_filename = "File not uploaded";
        }
    } //$abstracts_query_process == TRUE
    else
    {
        $url = l('Upload abstract', 'powersystems/pssp/abstract-code/upload');
        $abstracts_query_process_filename = "File not uploaded";
    }
    $download_om_pssp = l('Download power system simulation project','powersystems/pssp/full-download/project/'.$om_pssp_proposal_id);
    $return_html .= '<strong>Proposer Name:</strong><br />' . $abstracts_pro->name_title . ' ' . $abstracts_pro->contributor_name . '<br /><br />';
    $return_html .= '<strong>Title of the power system simulation Project:</strong><br />' . $abstracts_pro->project_title . '<br /><br />';
    $return_html .= '<strong>Other libraries used:</strong><br />' . $other_libraries_used . '<br /><br />';
    $return_html .= '<strong>Uploaded an abstract (brief outline) of the project:</strong><br />' . $abstract_filename . '<br /><br />';
    $return_html .= '<strong>Upload the  power system simulation for the developed process:</strong><br />' . $abstracts_query_process_filename . '<br /><br />';
    $return_html .= $download_om_pssp;
    return $return_html;
}