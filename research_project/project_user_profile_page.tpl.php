<?php
// $Id$
/**
 * Template to display user profile page.
 *
 * Fields available:
 * $person: user object
 * $module_path: the path of module folder in the syetem
 */
?>
<div id="user-card">
    <div id="user-card-left">
        <dl>
            <dt>Name</dt>
            <dd></dd>
        </dl>
        <dl>
            <dt>Age</dt>
            <dd></dd>
        </dl>
        <dl>
            <dt>Sex</dt>
            <dd></dd>
        </dl>
        <dl>
            <dt>Phone number</dt>
            <dd></dd>
        </dl>
        <dl>
            <dt>Email</dt>
            <dd><?=$person->mail?></dd>
        </dl>
        <dl>
            <dt>Memeber for</dt>
            <dd></dd>
        </dl>
    <?php if($person->access_edit_profile) { ?>
        <input type="button" value="Edit my profile" onclick="window.location.href='<?=url("user/profiles/{$person->uid}/edit")?>'"/>
    <?php } ?>
    </div>
    <div id="user-card-right">
        <h2>Research Projects</h2>
        <div id="user-card-right-info">
            <dl><dd>3</dd><dt>work for</dt></dl>
            <dl><dd>2</dd><dt>participate</dt></dl>
        </div>
    </div>
</div>