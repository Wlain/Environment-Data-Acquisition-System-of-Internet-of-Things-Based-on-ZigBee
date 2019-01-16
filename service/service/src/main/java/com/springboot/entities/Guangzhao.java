package com.springboot.entities;

import javax.persistence.*;
import java.util.Date;

/**
 * Created by 10497 on 2017/6/12.
 */
@Entity
@Table(name = "GZ")
public class Guangzhao {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;
    private int gz;
    private Date time;

    public void setId(int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    public void setGz(int gz) {
        this.gz = gz;
    }

    public int getGz() {
        return gz;
    }

    public void setTime(Date time) {
        this.time = time;
    }

    public Date getTime() {
        return time;
    }
}